const net = require('net');
const fs = require('fs');
const { execSync } = require('child_process');

const OPENROUTER_API_KEY = process.env.OPENROUTER_API_KEY;
if (!OPENROUTER_API_KEY) {
    console.error("Error: OPENROUTER_API_KEY is not defined in the environment!");
    process.exit(1);
}

const SERIAL_PORT = 4444;

function cleanOutputText(text) {
    if (!text) return "";
    return text
        .replace(/[\u2018\u2019]/g, "'") // Left/Right curly apostrophes -> '
        .replace(/[\u201C\u201D]/g, '"') // Left/Right curly double quotes -> "
        .replace(/[\u2013\u2014]/g, "-") // En/Em dashes -> -
        .replace(/\u2022/g, "*")         // Bullet point -> *
        .replace(/\u2026/g, "...");      // Ellipsis -> ...
}

console.log("Starting Agentic x86 Assembly Compiler serial bridge...");

// Maintain Chat History/Memory for the Agent
let chatHistory = [
    { 
        role: "system", 
        content: "You are the resident AI coding agent of rejectDOS, a custom 16-bit real-mode operating system. You converse naturally with the user and maintain a friendly assistant posture. If the user asks you to carry out physical tasks like clearing the screen, shifting video colors, or speaking over the PC speaker, you can run an assembly compilation tool! To invoke the tool, return a [TOOL:EXEC_ASM Description=\"<Simple 1-sentence action summary>\"] block, followed by the assembly lines, and end with [/TOOL]. Keep standard comments outside the tool block extremely short (under 2 sentences).\n\nCRITICAL TOOL PRINCIPLES:\n1. We are in 16-bit Real Mode (8086/186/286/386 compatible).\n2. Write pure flat, unsegmented nasm syntax. DO NOT include section or org statements.\n3. You MUST end your machine code with a RETF (Far Return, 0xCB) so control returns safely back to the OS!\n4. Keep registers saved (push/pop) if you modify segment pointers. Segment 1000h holds our core OS, Segment 2000h is loaded.\n5. You MUST write exactly ONE instruction per line. DO NOT group multiple instructions. Every statement must have its own newline.\n6. For loops or delays, use simple register decrements.\n\nExample to clear screen:\n[TOOL:EXEC_ASM Description=\"Clears the active screen text and wraps the prompt to the top-left coordinate\"]\nmov ah, 06h\nmov al, 00h\nmov bh, 07h\nmov cx, 0000h\nmov dx, 184Fh\nint 10h\nmov ah, 02h\nmov bh, 00h\nmov dx, 0000h\nint 10h\nretf\n[/TOOL]" 
    }
];

function compileAssembly(asmCode) {
    console.log("[Bridge] Sanitizing and compiling ASM payload via NASM...");
    const tempAsmPath = "/tmp/ai_code.asm";
    const tempBinPath = "/tmp/ai_code.bin";

    if (fs.existsSync(tempAsmPath)) fs.unlinkSync(tempAsmPath);
    if (fs.existsSync(tempBinPath)) fs.unlinkSync(tempBinPath);

    // Dynamic split-line sanitizer
    let sanitizedCode = asmCode;
    const opcodes = ["mov", "int", "push", "pop", "ret", "retf", "xor", "add", "sub", "dec", "inc", "out", "in", "jmp", "jnz", "jz", "je", "jne", "cmp", "lea", "call"];
    
    let lines = sanitizedCode.split('\n');
    let dynamicAsm = [];
    for (let line of lines) {
        let trimmed = line.trim();
        if (!trimmed) continue;
        
        let words = trimmed.split(/\s+/);
        let currentInstruction = [];
        
        for (let word of words) {
            let cleanWord = word.toLowerCase().replace(/[,:;]/g, "");
            if (opcodes.includes(cleanWord) && currentInstruction.length > 0) {
                dynamicAsm.push(currentInstruction.join(" "));
                currentInstruction = [word];
            } else {
                currentInstruction.push(word);
            }
        }
        if (currentInstruction.length > 0) {
            dynamicAsm.push(currentInstruction.join(" "));
        }
    }
    sanitizedCode = dynamicAsm.join('\n');
    fs.writeFileSync(tempAsmPath, sanitizedCode, 'utf-8');

    try {
        execSync(`nasm -f bin ${tempAsmPath} -o ${tempBinPath}`);
        if (!fs.existsSync(tempBinPath)) {
            throw new Error("NASM compilation completed, but no binary file was created.");
        }
        const binData = fs.readFileSync(tempBinPath);
        return { success: true, data: binData };
    } catch (err) {
        return { success: false, error: err.message };
    }
}

function connectToSerial() {
    const client = net.createConnection({ port: SERIAL_PORT, host: '127.0.0.1' }, () => {
        console.log("Connected to QEMU Virtual Serial COM1 on Port 4444!");
    });

    let buffer = "";

    client.on('data', async (data) => {
        const text = data.toString('utf-8');
        
        // Check if we are receiving high-priority user response confirmation of a tool call
        if (text === 'y' || text === 'n') {
            console.log(`[OS -> Bridge] User pressed execution confirmation: "${text}"`);
            return; // Handled directly in the tool loop state below
        }

        buffer += text;
        if (buffer.includes('\n')) {
            const prompt = buffer.trim();
            buffer = ""; // Clear buffer
            
            console.log(`\n[OS -> Serial] received query: "${prompt}"`);
            chatHistory.push({ role: "user", content: prompt });
            
            try {
                console.log("[Bridge] Dispatching query to OpenRouter...");
                const response = await fetch("https://openrouter.ai/api/v1/chat/completions", {
                    method: "POST",
                    headers: {
                        "Authorization": `Bearer ${OPENROUTER_API_KEY}`,
                        "Content-Type": "application/json",
                        "HTTP-Referer": "https://rejectdos.org",
                        "X-Title": "rejectDOS 16-bit OS"
                    },
                    body: JSON.stringify({
                        model: "openai/gpt-5.5",
                        messages: chatHistory
                    })
                });

                const result = await response.json();
                let reply = result.choices?.[0]?.message?.content || "Error: Empty response from model.";
                chatHistory.push({ role: "assistant", content: reply });
                
                console.log(`[LLM Agent Response]:\n${reply}\n-------------------`);

                // Parse for [TOOL:EXEC_ASM Description="..."]...[/TOOL]
                const toolMatch = reply.match(/\[TOOL:EXEC_ASM\s+Description="([\s\S]*?)"\]([\s\S]*?)\[\/TOOL\]/i);
                
                if (toolMatch) {
                    const description = toolMatch[1].trim();
                    const asmCode = toolMatch[2].trim();

                    // Strip tool block from standard conversational output text reply
                    const chatText = reply.replace(/\[TOOL:EXEC_ASM[\s\S]*?\[\/TOOL\]/gi, "").trim();

                    // 1. Send conversational text first (standard text streaming)
                    if (chatText) {
                        client.write(cleanOutputText(chatText) + "\n");
                    }

                    // 2. Send Tool escape trigger: ESC (0x1B) + 'T' (0x54) 
                    const header = Buffer.alloc(2);
                    header[0] = 0x1B; // ESC
                    header[1] = 0x54; // 'T' for Tool Call
                    client.write(header);

                    // 3. Write description ending with a single newline
                    client.write(cleanOutputText(description) + "\n");
                    
                    // 4. Write Assembly Body lines and end with \x00 (Null terminator)
                    client.write(asmCode + "\n\x00");

                    // 5. Setup synchronous user y/n confirmation listener over TCP
                    const handleConfirmation = (confirmData) => {
                        const choice = confirmData.toString('utf-8').trim().toLowerCase();
                        client.removeListener('data', handleConfirmation); // Remove hook

                        if (choice === 'y') {
                            console.log("[Bridge] User authorized payload execution. Compiling...");
                            const compileResult = compileAssembly(asmCode);

                            if (compileResult.success) {
                                const payloadSize = compileResult.data.length;
                                const binaryHeader = Buffer.alloc(4);
                                binaryHeader[0] = 0x1B; // ESC
                                binaryHeader[1] = 0x58; // 'X' execution trigger
                                binaryHeader[2] = payloadSize & 0xFF; // Low byte
                                binaryHeader[3] = (payloadSize >> 8) & 0xFF; // High byte

                                console.log(`[Bridge] Sending binary header and ${payloadSize} machine bytes...`);
                                client.write(binaryHeader);
                                client.write(compileResult.data);
                            } else {
                                console.error("[Bridge] Payload compilation failed:", compileResult.error);
                                client.write(`AI Compilation Error:\n${compileResult.error}\n`);
                            }
                        } else {
                            console.log("[Bridge] User aborted payload execution.");
                        }

                        // Always terminate the stream with EOT to give control back to terminal prompt
                        client.write(Buffer.from([0x04]));
                    };

                    client.on('data', handleConfirmation);

                } else {
                    // Normal conversational reply with no tool call
                    client.write(cleanOutputText(reply) + "\n\x04");
                }
                
            } catch (err) {
                console.error("Bridge Error: ", err);
                client.write(`Bridge Error: ${err.message}\n\x04`);
            }
        }
    });

    client.on('error', (err) => {
        console.log("Waiting for QEMU to start up or serial connection error... retrying in 2s");
        setTimeout(connectToSerial, 2000);
    });

    client.on('end', () => {
        console.log("Serial connection closed. Retrying...");
        setTimeout(connectToSerial, 2000);
    });
}

// Join loop
connectToSerial();
