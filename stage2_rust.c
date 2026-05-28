#include "term.c"

#define INPUT_SIZE 64
#define PORT_COM1 0x3F8

// Compiler inline assembly instructions via Watcom auxiliary pragmas
int inp(unsigned int port);
#pragma aux inp = \
    "xor ax, ax" \
    "in al, dx" \
    parm [dx] \
    value [ax];

void outp(unsigned int port, int val);
#pragma aux outp = \
    "out dx, al" \
    parm [dx] [ax];

// Fast no-dependency string equality check
int str_eq(char *s1, char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

char input[INPUT_SIZE];
short input_len;

static unsigned int prng_seed = 0xACE1; // Default seed

// Seed PRNG from BIOS clock ticks (0040:006C in real-mode segment)
void seed_random(void) {
    unsigned int far *timer_ptr = (unsigned int far *)0x0040006C;
    if (*timer_ptr != 0) {
        prng_seed = *timer_ptr;
    }
}

// LCG Pseudo-Random Number Generator (16-bit limit multiplier)
unsigned int rand_16(void) {
    prng_seed = (prng_seed * 25173 + 13849);
    return prng_seed;
}

// Roll N dice of S sides and sum results
unsigned int roll_dice(unsigned int count, unsigned int sides) {
    unsigned int sum = 0;
    unsigned int i;
    if (sides == 0) return 0;
    if (count == 0) count = 1;
    
    seed_random();
    
    for (i = 0; i < count; i++) {
        sum += (rand_16() % sides) + 1;
    }
    return sum;
}

// Native 16-bit Real Mode Evaluator in C supporting sequential ops, division safety, and dice notation
unsigned int eval(char *input_ptr, short len) {
    char *c = input_ptr;
    char *end = input_ptr + len;
    unsigned int res = 0;
    unsigned int temp = 0;
    char op = '+';
    
    while (c < end) {
        char ch = *c;
        if (ch >= '0' && ch <= '9') {
            temp = temp * 10 + (ch - '0');
        } else if (ch == 'd' || ch == 'D') {
            unsigned int count = temp;
            unsigned int sides = 0;
            if (count == 0) {
                count = 1;
            }
            c++;
            while (c < end && *c >= '0' && *c <= '9') {
                sides = sides * 10 + (*c - '0');
                c++;
            }
            c--;
            temp = roll_dice(count, sides);
        } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%') {
            if (op == '+') res += temp;
            else if (op == '-') res -= temp;
            else if (op == '*') res *= temp;
            else if (op == '/') {
                if (temp != 0) res /= temp;
                else res = 0;
            }
            else if (op == '%') {
                if (temp != 0) res %= temp;
                else res = 0;
            }
            temp = 0;
            op = ch;
        }
        c++;
    }
    
    if (op == '+') res += temp;
    else if (op == '-') res -= temp;
    else if (op == '*') res *= temp;
    else if (op == '/') {
        if (temp != 0) res /= temp;
        else res = 0;
    }
    else if (op == '%') {
        if (temp != 0) res %= temp;
        else res = 0;
    }
    
    return res;
}

// Initialize serial port COM1 to 9600 baud, 8N1, no interrupts
void init_serial(void) {
    outp(PORT_COM1 + 1, 0x00);    // Disable all interrupts
    outp(PORT_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outp(PORT_COM1 + 0, 0x0C);    // Set divisor to 12 (9600 baud)
    outp(PORT_COM1 + 1, 0x00);    // High byte of divisor
    outp(PORT_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
    outp(PORT_COM1 + 2, 0xC7);    // Enable FIFO, clear them
    outp(PORT_COM1 + 4, 0x0B);    // RTS/DTR set
}

// Check if serial data is ready
int serial_received(void) {
    return inp(PORT_COM1 + 5) & 1;
}

// Read serial byte with strict hardware tick timeout (returns - 1 on timeout)
int read_serial_timeout(unsigned int max_ticks) {
    unsigned int far *timer_ptr = (unsigned int far *)0x0040006C;
    unsigned int start_ticks = *timer_ptr;
    
    while (serial_received() == 0) {
        unsigned int current_ticks = *timer_ptr;
        unsigned int delta = current_ticks - start_ticks;
        if (delta > max_ticks) {
            return -1; // Timeout threshold reached
        }
    }
    return inp(PORT_COM1);
}

// Write byte over serial UART
void write_serial(char a) {
    while ((inp(PORT_COM1 + 5) & 0x20) == 0);
    outp(PORT_COM1, a);
}

// Write string over serial UART
void print_serial(char *str) {
    while (*str) {
        write_serial(*str);
        str++;
    }
}

// Write a character on-screen with precise coordinate attributes (BIOS INT 10h, AH=09h)
void putc_attr(char c, char attr) {
    __asm {
        mov ah, 0x09
        mov al, c
        mov bh, 00h
        mov bl, attr       ; Color attribute
        mov cx, 0x01       ; Write 1 character
        int 10h
        
        ; Now advance cursor with silent write
        mov ah, 0x0E
        mov al, c
        mov bh, 00h
        int 10h
    }
}

// Simple syntax-highlight printer for assembly lines
void print_highlight(char *line) {
    char *c = line;
    char word[32];
    short idx = 0;
    short j;
    char color;
    
    // Check if line is a comment
    while (*c == ' ' || *c == '\t') {
        putc(*c);
        c++;
    }
    if (*c == ';') {
        while (*c) {
            putc_attr(*c, 8);
            c++;
        }
        return;
    }
    
    // Parse word by word
    while (*c) {
        if (*c == ' ' || *c == ',' || *c == '\t' || *c == ';' || *c == '\r' || *c == '\n') {
            word[idx] = '\0';
            if (idx > 0) {
                color = 7; // Default: Light Gray
                
                // Opcode check
                if (str_eq(word, "mov") || str_eq(word, "int") || 
                    str_eq(word, "push") || str_eq(word, "pop") || 
                    str_eq(word, "retf") || str_eq(word, "xor") || 
                    str_eq(word, "add") || str_eq(word, "sub") || 
                    str_eq(word, "dec") || str_eq(word, "inc") || 
                    str_eq(word, "out") || str_eq(word, "in") || 
                    str_eq(word, "cmp") || str_eq(word, "jmp")) {
                    color = 14; // Bright Yellow for Opcodes
                }
                else if (word[idx - 1] == 'h' || (word[0] == '0' && word[1] == 'x')) {
                    color = 10; // Light Green for Hex values
                }
                
                // Print buffered word in color
                j = 0;
                while (word[j]) {
                    putc_attr(word[j], color);
                    j++;
                }
                idx = 0;
            }
            
            // Print delimiter
            if (*c == ';') {
                while (*c) {
                    putc_attr(*c, 8);
                    c++;
                }
                break;
            } else {
                putc(*c);
            }
        } else {
            if (idx < 31) {
                word[idx++] = *c;
            }
        }
        c++;
    }
    
    // Print remain
    if (idx > 0) {
        word[idx] = '\0';
        color = 7;
        if (str_eq(word, "retf")) color = 14;
        
        j = 0;
        while (word[j]) {
            putc_attr(word[j], color);
            j++;
        }
    }
}

// Securely load raw compiled x86 machine bytes into segment 2000h:0000h and execute!
int load_and_execute_payload(unsigned int size) {
    char far *loader_ptr = (char far *)(((unsigned long)0x2000 << 16) | 0x0000);
    void (far *run_payload)(void) = (void (far *)(void))(((unsigned long)0x2000 << 16) | 0x0000);
    unsigned int i;
    for (i = 0; i < size; i++) {
        int next_byte = read_serial_timeout(55); // 3 seconds timeout for streaming bytes
        if (next_byte == -1) {
            print("\r\n[Fatal: Payload transmission timeout! Skip.]\r\n");
            return 0; // Abort
        }
        *loader_ptr = (char)next_byte;
        loader_ptr++;
    }
    run_payload();
    return 1; // Success
}

// Handle Tool Call parsing and execution confirmation
void handle_tool_call(void) {
    char buf[128];
    short idx = 0;
    int next_char;
    char c;
    
    // 1. Read and print description string (terminated by newline)
    print("\r\n--- AI Agent Tool Request ---\r\n");
    print("Action proposed: ");
    while (1) {
        next_char = read_serial_timeout(91); // 5 seconds timeout for description stream
        if (next_char == -1) {
            print("\r\n[Error: Description read timeout. Aborting.]\r\n");
            return;
        }
        c = (char)next_char;
        if (c == '\n') {
            print("\r\n");
            break;
        }
        putc_attr(c, 11); // Print description in Cyan (Color 11)
    }
    
    // 2. Read and print highlighted assembly code block (until terminated by \x00)
    print("\r\n[ASM Payload Draft]:\r\n");
    print("----------------------------------------\r\n");
    while (1) {
        next_char = read_serial_timeout(91); // 5 seconds timeout for assembly body
        if (next_char == -1) {
            print("\r\n[Error: Assembly block read timeout. Aborting.]\r\n");
            return;
        }
        c = (char)next_char;
        if (c == 0) {
            break; // Finished reading assembly body
        }
        if (c == '\n') {
            buf[idx] = '\0';
            print_highlight(buf);
            print("\r\n");
            idx = 0;
        } else {
            if (idx < 127) {
                buf[idx++] = c;
            }
        }
    }
    print("----------------------------------------\r\n");
    
    // 3. User confirmation loop
    while (1) {
        print("Execute this assembly code? (y/n): ");
        c = getc();
        putc(c); // Echo key
        print("\r\n");
        if (c == 'y' || c == 'Y') {
            write_serial('y'); // Signal YES to the compiler bridge
            
            // Wait for compiled binary header (ESC + 'X' + size)
            next_char = read_serial_timeout(182); // 10 seconds timeout for compilation
            if (next_char == 0x1B) {
                int cmd_id = read_serial_timeout(36);
                if (cmd_id == 'X') {
                    int size_low = read_serial_timeout(36);
                    int size_high = read_serial_timeout(36);
                    if (size_low != -1 && size_high != -1) {
                        unsigned int bin_size = (unsigned char)size_low | ((unsigned char)size_high << 8);
                        print("Compiling and launching payload...\r\n");
                        load_and_execute_payload(bin_size);
                        print("Payload finished execution. Returning to shell.\r\n");
                    }
                }
            } else {
                print("[Error: No compilation header received. Aborting.]\r\n");
            }
            break;
        } else if (c == 'n' || c == 'N') {
            write_serial('n'); // Signal NO to the compiler bridge
            print("Aborted. Code execution skipped.\r\n");
            break;
        } else {
            print("Invalid typing. Choose y or n.\r\n");
        }
    }
}

// Send prompts to LLM and retrieve streaming text back over serial
void ask_ai(void) {
    int next_char;
    char c;
    unsigned int far *timer_ptr = (unsigned int far *)0x0040006C;
    unsigned int start_ticks = *timer_ptr;
    unsigned int max_wait = 364; // Wait up to 20 seconds for the first character response
    
    // Write entire prompt message over serial
    print_serial(input);
    write_serial('\n'); // Trigger line detection on bridge
    
    // Receive and echo characters until End of Transmission (EOT)
    while (1) {
        // Safe timeout block waiting for serial ready
        while (serial_received() == 0) {
            unsigned int current_ticks = *timer_ptr;
            unsigned int delta = current_ticks - start_ticks;
            if (delta > max_wait) {
                print("\r\n[Error: AI Agent response timeout. Rescheduling...]\r\n");
                return; // Return safely to terminal!
            }
        }
        
        c = inp(PORT_COM1);
        
        // Reset timers dynamically and shrink the window to 3 seconds for streaming
        start_ticks = *timer_ptr;
        max_wait = 55;
        
        if (c == 0x1B) { // ESC!
            int cmd_id = read_serial_timeout(36); // Wait up to 2 seconds for command ID
            if (cmd_id == 'T') { // Tool Call!
                handle_tool_call();
                // Reset timer limits after tool returns
                start_ticks = *timer_ptr;
                max_wait = 55;
            }
        } else if (c == 4) { // EOT (ASCII End of Transmission) marker
            break;
        } else if (c == '\n') {
            print("\r\n");
        } else {
            putc(c);
        }
    }
    print("\r\n");
}

void eval_shell(void) {
    // Check if input begins with '!' for built-in or math actions
    if (input[0] == '!') {
        if (input[1] == 'h' && input[2] == 'e' && input[3] == 'l' && input[4] == 'p' && input[5] == '\0') {
            print("rejectDOS Help:\r\n");
            print("  <text> : Chat with the LLM Agent (default)\r\n");
            print("  !<expr>: Solve math or roll dice (e.g. !2d6+5, !10*5)\r\n");
            print("  !help  : Show this help message\r\n");
        } else {
            // Evaluate mathematical/dice expressions (starting index 1, skip '!')
            unsigned int res = eval(&input[1], input_len - 1);
            print("= ");
            printnum(res);
            print("\r\n");
        }
    } else {
        // Default to routing directly through LLM
        if (input_len > 0) {
            ask_ai();
        }
    }
}

int main(void) {
    char c;
    seed_random(); // Seed PRNG
    init_serial(); // Initialize COM1 UART Serial Port
    
    // Boot help welcome banner
    print("Welcome to rejectDOS!\r\n");
    print("  Type anything to converse with the LLM Agent.\r\n");
    print("  Use ! prefix to execute math or dice (e.g. !2d20, !10*5).\r\n");
    print("  Type !help for info.\r\n\r\n");
    
    while (1) {
        print("> ");
        input_len = 0;
        while (1) {
            c = getc();
            if (c == '\r') {
                print("\r\n");
                input[input_len] = '\0';
                eval_shell();
                break;
            } else if (c == 127 || c == 8) {
                if (input_len > 0) {
                    input[--input_len] = '\0';
                    backspace();
                }
            } else {
                if (input_len < INPUT_SIZE - 1) {
                    // Allow printable ASCII characters so rich questions can be typed in!
                    if (c >= 32 && c <= 126) {
                        input[input_len++] = c;
                        putc(c);
                    }
                }
            }
        }
    }
}
