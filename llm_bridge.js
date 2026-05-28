const net = require('net');

const OPENROUTER_API_KEY = process.env.OPENROUTER_API_KEY;
if (!OPENROUTER_API_KEY) {
    console.error("Error: OPENROUTER_API_KEY is not defined in the environment!");
    process.exit(1);
}

const SERIAL_PORT = 4444;

console.log("Starting Low-Level LLM serial bridge...");

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
            
            console.log(`\n[OS -> Serial] received prompt: "${prompt}"`);
            
            try {
                console.log("[Bridge] Dispatching query to OpenRouter...");
                const response = await fetch("https://openrouter.ai/api/v1/chat/completions", {
                    method: "POST",
                    headers: {
                        "Authorization": `Bearer ${OPENROUTER_API_KEY}`,
                        "Content-Type": "application/json",
                        "HTTP-Referer": "https://rejectdos.org", // Optional
                        "X-Title": "rejectDOS 16-bit OS" // Optional
                    },
                    body: JSON.stringify({
                        model: "openrouter/free",
                        messages: [
                            { role: "system", content: "You are an AI assistant running inside a 16-bit real-mode custom Operating System called rejectDOS. Keep answers extremely short and concise (under 2-3 sentences), with plain ASCII characters, because terminal space is limited. Avoid markdown formatting." },
                            { role: "user", content: prompt }
                        ]
                    })
                });

                const result = await response.json();
                const reply = result.choices?.[0]?.message?.content || "Error: Empty response from model.";
                
                console.log(`[Serial -> OS] replying: "${reply}"`);
                
                // Write reply line-by-line of ASCII, and end with the EOT (\x04) marker
                client.write(reply + "\n\x04");
                
            } catch (err) {
                console.error("Bridge Error: ", err);
                client.write(`Bridge Connection Error: ${err.message}\n\x04`);
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
