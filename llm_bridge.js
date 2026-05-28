const net = require('net');
const fs = require('fs');
const { execSync } = require('child_process');

const OPENROUTER_API_KEY = process.env.OPENROUTER_API_KEY;
if (!OPENROUTER_API_KEY) {
    console.error("Error: OPENROUTER_API_KEY is not defined in the environment!");
    process.exit(1);
}

const SERIAL_PORT = 4444;

console.log("Starting Advanced LLM-to-x86 Assembly Compiler serial bridge...");

function compileAssembly(asmCode) {
    console.log("[Bridge] Sanitizing and compiling ASM payload via NASM...");
    const tempAsmPath = "/tmp/ai_code.asm";
    const tempBinPath = "/tmp/ai_code.bin";

    // Clean up previous files if any
    if (fs.existsSync(tempAsmPath)) fs.unlinkSync(tempAsmPath);
    if (fs.existsSync(tempBinPath)) fs.unlinkSync(tempBinPath);

    // Auto-sanitizer: If the model grouped multiple instructions on a single line, 
    // split them by searching for inline opcodes (e.g. "mov", "int", "push", "pop", "retf") 
    // preceded by space and insert linebreaks!
    let sanitizedCode = asmCode;
    const opcodes = ["mov", "int", "push", "pop", "ret", "retf", "xor", "add", "sub", "dec", "inc", "out", "in", "jmp", "jnz", "jz", "je", "jne", "cmp", "lea", "call"];
    
    // Split combined instruction strings
    let lines = sanitizedCode.split('\n');
    let dynamicAsm = [];
    for (let line of lines) {
        let trimmed = line.trim();
        if (!trimmed) continue;
        
        // Find individual instructions in case they were joined by spaces (e.g. "mov ah, 06h mov al, 00h")
        // We tokenise and look for opcode triggers
        let words = trimmed.split(/\s+/);
        let currentInstruction = [];
        
        for (let word of words) {
            // Clean comma-attached words to find clean triggers (e.g. "retf" or "xor")
            let cleanWord = word.toLowerCase().replace(/[,:;]/g, "");
            
            if (opcodes.includes(cleanWord) && currentInstruction.length > 0) {
                // We hit a new opcode on the same line! Flush the previous instruction
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
    console.log(`[Bridge] Sanitized Assembly code:\n${sanitizedCode}\n-------------------`);

    // Write sanitized assembly out
    fs.writeFileSync(tempAsmPath, sanitizedCode, 'utf-8');

    try {
        // Compile to pure flat binary
        execSync(`nasm -f bin ${tempAsmPath} -o ${tempBinPath}`);
        if (!fs.existsSync(tempBinPath)) {
            throw new Error("NASM compilation completed, but no binary file was created.");
        }
        const binData = fs.readFileSync(tempBinPath);
        console.log(`[Bridge] NASM compilation successful! Generated ${binData.length} bytes of machine code.`);
        return { success: true, data: binData };
    } catch (err) {
        console.error("[Bridge] NASM compilation failed:", err.message);
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
        buffer += text;

        if (buffer.includes('\n')) {
            const prompt = buffer.trim();
            buffer = ""; // Clear buffer
            
            console.log(`\n[OS -> Serial] received query: "${prompt}"`);
            
            try {
                console.log("[Bridge] Dispatching compiler request to OpenRouter...");
                const response = await fetch("https://openrouter.ai/api/v1/chat/completions", {
                    method: "POST",
                    headers: {
                        "Authorization": `Bearer ${OPENROUTER_API_KEY}`,
                        "Content-Type": "application/json",
                        "HTTP-Referer": "https://rejectdos.org",
                        "X-Title": "rejectDOS 16-bit OS"
                    },
                    body: JSON.stringify({
                        model: "openrouter/free",
                        messages: [
                            { 
                                role: "system", 
                                content: "You are the AI chief engineer of rejectDOS, a 16-bit real-mode custom OS. When a user asks you to interact with local hardware, change screen parameters, play sound, clear the screen, read disk directories, or execute a task, you can write native real-mode x86 assembly to do it! To execute arbitrary code, output an [ASM] block ending with [/ASM]. Keep standard text descriptions outside the block exceedingly short, as screen buffer space is small in real-mode terminal.\n\nCRITICAL ASSEMBLY PRINCIPLES:\n1. We are in 16-bit Real Mode (8086/286/386 compatible).\n2. Write pure flat, unsegmented nasm syntax.\n3. The code will be compiled to raw binary. DO NOT include section or org statements.\n4. You MUST end your machine code with a RETF (Far Return, 0xCB) so control is safely handed back to the OS shell!\n5. Keep registers saved (push/pop) if you modify segment pointers. Segment 2000h:0000h is free for your sandbox code.\n6. You MUST write exactly ONE instruction per line. DO NOT group multiple instructions on a single line. Every statement (like mov, int, push, pop, retf) must have its own dedicated newline.\n\nExample to clear screen:\n[ASM]\nmov ah, 06h\nmov al, 00h\nmov bh, 07h\nmov cx, 0000h\nmov dx, 184Fh\nint 10h\nmov ah, 02h\nmov bh, 00h\nmov dx, 0000h\nint 10h\nretf\n[/ASM]" 
                            },
                            { role: "user", content: prompt }
                        ]
                    })
                });

                const result = await response.json();
                let reply = result.choices?.[0]?.message?.content || "Error: Empty response from model.";
                
                console.log(`[LLM Response]:\n${reply}\n-------------------`);

                // Parse for [ASM]...[/ASM] block
                const asmMatch = reply.match(/\[ASM\]([\s\S]*?)\[\/ASM\]/i);
                if (asmMatch) {
                    const asmCode = asmMatch[1].trim();
                    const compileResult = compileAssembly(asmCode);

                    if (compileResult.success) {
                        // Strip [ASM] block from the chat reply so it doesnt print on screen
                        const textReply = reply.replace(/\[ASM\][\s\S]*?\[\/ASM\]/gi, "").trim();
                        
                        // Send text reply to OS terminal first
                        if (textReply) {
                            client.write(textReply + "\n");
                        }
                        
                        // Send the Escape execution instruction: 0x1B (ESC) + 'X' (0x58) + 16-bit size + Bytes
                        const size = compileResult.data.length;
                        const header = Buffer.alloc(4);
                        header[0] = 0x1B; // ESC
                        header[1] = 0x58; // 'X'
                        header[2] = size & 0xFF; // Low byte of size
                        header[3] = (size >> 8) & 0xFF; // High byte of size

                        console.log(`[Bridge] Streaming header and ${size} bytes of binary payload...`);
                        client.write(header);
                        client.write(compileResult.data);
                        client.write(Buffer.from([0x04])); // Terminate with EOT
                        
                    } else {
                        // If compilation fails, broadcast compilation error back to terminal screen
                        client.write(`AI Code Compiler Error:\n${compileResult.error}\n\x04`);
                    }
                } else {
                    // Normal text conversation with no code execution
                    client.write(reply + "\n\x04");
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
