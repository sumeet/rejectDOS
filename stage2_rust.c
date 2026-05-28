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

// Read single character from serial (wait until ready)
char read_serial(void) {
    while (serial_received() == 0);
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

// Securely load raw compiled x86 machine bytes into segment 2000h:0000h and execute!
void load_and_execute_payload(unsigned int size) {
    // Construct segmented far pointers using explicit segment/offset math to prevent compiler cast truncation
    char far *loader_ptr = (char far *)(((unsigned long)0x2000 << 16) | 0x0000);
    void (far *run_payload)(void) = (void (far *)(void))(((unsigned long)0x2000 << 16) | 0x0000);
    unsigned int i;
    
    // Read exact payload bytes from hardware FIFO buffer and copy directly into memory sandbox
    for (i = 0; i < size; i++) {
        *loader_ptr = read_serial();
        loader_ptr++;
    }
    
    // Far Call the payload. When compiled code finishes with RETF, control returns right back!
    run_payload();
}

// Send prompts to LLM and retrieve streaming text back over serial
void ask_ai(void) {
    char c;
    
    // Write entire prompt message over serial
    print_serial(input);
    write_serial('\n'); // Trigger line detection on bridge
    
    // Receive and echo characters until End of Transmission (EOT)
    while (1) {
        if (serial_received()) {
            c = inp(PORT_COM1);
            if (c == 0x1B) { // ESC!
                // Read next byte to ensure Command ID match 'X' (0x58)
                char cmd_id = read_serial();
                if (cmd_id == 'X') {
                    // Read 16-bit little-endian binary size
                    unsigned char size_low = read_serial();
                    unsigned char size_high = read_serial();
                    unsigned int bin_size = size_low | (size_high << 8);
                    
                    // Route bytes directly to segment execution loader
                    load_and_execute_payload(bin_size);
                }
            } else if (c == 4) { // EOT (ASCII End of Transmission) marker
                break;
            } else if (c == '\n') {
                print("\r\n");
            } else {
                putc(c);
            }
        }
    }
    print("\r\n");
}

void eval_shell(void) {
    // Check if input begins with '!' for built-in or math actions
    if (input[0] == '!') {
        if (input[1] == 'h' && input[2] == 'e' && input[3] == 'l' && input[4] == 'p' && input[5] == '\0') {
            print("rejectDOS Help:\r\n");
            print("  <text> : Chat in real-time with LLM (default)\r\n");
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
    print("  Type anything to converse with the LLM.\r\n");
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
