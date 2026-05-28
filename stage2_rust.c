#include "term.c"

#define INPUT_SIZE 64
#define PORT_COM1 0x3F8

// Compiler inline assembly instructions via Watcom auxiliary pragmas
int inp(unsigned int port);
#pragma aux inp = \
    "in al, dx" \
    parm [dx] \
    value [ax];

int outp(unsigned int port, int val);
#pragma aux outp = \
    "out dx, al" \
    parm [dx] [ax] \
    value [ax];

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

// Send prompts to LLM and retrieve streaming text back over serial
void ask_ai(void) {
    char c;
    print("Connecting to OpenRouter LLM...\r\n");
    
    // Write prompt message over serial
    print_serial(&input[3]);
    write_serial('\n'); // Trigger line detection on bridge
    
    // Receive and echo characters until End of Transmission (EOT)
    while (1) {
        if (serial_received()) {
            c = inp(PORT_COM1);
            if (c == 4) { // EOT (ASCII End of Transmission) marker
                break;
            }
            if (c == '\n') {
                print("\r\n");
            } else {
                putc(c);
            }
        }
    }
    print("\r\n");
}

void eval_shell(void) {
    // Check if input begins with "ai " prefix
    if (input[0] == 'a' && input[1] == 'i' && input[2] == ' ') {
        ask_ai();
    } else {
        // Evaluate mathematical operations
        unsigned int res = eval(input, input_len);
        print("= ");
        printnum(res);
    }
}

int main(void) {
    char c;
    seed_random(); // Seed PRNG
    init_serial(); // Initialize COM1 UART Serial Port
    
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
