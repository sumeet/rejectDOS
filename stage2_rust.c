#include "term.c"

#define INPUT_SIZE 64

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
    if (count == 0) count = 1; // "d6" defaults to "1d6"
    
    // Seed clock variations dynamically before rolls
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
            // Dice rolling parser: 'temp' is the count (or default to 1 if temp == 0)
            unsigned int count = temp;
            unsigned int sides = 0;
            if (count == 0) {
                count = 1; // Default to 1 if no leading number (e.g. "d6")
            }
            c++; // Skip 'd'
            
            // Read sides of the die
            while (c < end && *c >= '0' && *c <= '9') {
                sides = sides * 10 + (*c - '0');
                c++;
            }
            c--; // Adjust index for outer loop iteration increment
            
            // Roll dice and store result in temp
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

void eval_shell() {
    unsigned int res = eval(input, input_len);
    print("= ");
    printnum(res);
}

int main(void) {
    char c;
    seed_random(); // Seed randomly once at bootloader start
    
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
                    // Allow letters 'd' and 'D' in addition to numbers and operators
                    if ((c >= '0' && c <= '9') || c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == 'd' || c == 'D') {
                        input[input_len++] = c;
                        putc(c);
                    }
                }
            }
        }
    }
}
