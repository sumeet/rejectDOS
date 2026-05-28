#include "term.c"

#define INPUT_SIZE 32

char input[INPUT_SIZE];
short input_len;

// Pad args to 32-bit unsigned long to match the 32-bit Rust Stack alignment
extern unsigned int __cdecl eval(unsigned long input_ptr_32, unsigned long input_len_32);

void eval_shell() {
    // Cast variables to 32-bit unsigned longs to push exactly 4 bytes each
    unsigned long input_ptr_32 = (unsigned long)(unsigned int)input;
    unsigned long input_len_32 = (unsigned long)(unsigned int)input_len;
    
    unsigned int res = eval(input_ptr_32, input_len_32);
    print("= ");
    printnum(res);
}

int main(void) {
    char c;
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
                    input[input_len++] = c;
                    putc(c);
                }
            }
        }
    }
}
