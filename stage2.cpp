// GOD TUTORIAL:
// https://sebastian.graphics/blog/16-bit-tiny-model-standalone-c-with-open-watcom.html

#include "term.cpp"

#define INPUT_SIZE 64

char input[INPUT_SIZE];
short input_len;

void eval() {
    char *c = input;
    unsigned int res = 0;
    unsigned int temp = 0;
    char op = '+';
    
    while (*c) {
        if (*c >= '0' && *c <= '9') {
            temp = temp * 10 + (*c - '0');
        } else if (*c == '+' || *c == '-') {
            if (op == '+') res += temp;
            else if (op == '-') res -= temp;
            temp = 0;
            op = *c;
        }
        c++;
    }
    if (op == '+') res += temp;
    else if (op == '-') res -= temp;
    
    print("= ");
    printnum(res);
}

int main(void) {
    while (1) {
        print("> ");
        int cmd_done = 0;
        input_len = 0;
        while (!cmd_done) {
            char c = getc();
            switch (c) {
                case '\r':
                    cmd_done = 1;
                    print("\r\n");
                    input[input_len] = '\0'; // Fix: Ensure null-terminated
                    eval();
                    break;
                case 127:
                    if (input_len > 0) {
                        input[--input_len] = '\0';
                        backspace();
                    }
                    break;
                default:
                    if (input_len < INPUT_SIZE - 1) {
                        input[input_len++] = c;
                        putc(c);
                    }
                    break;
            }
        }
    }
}

// for some reason the linker keeps looking for this, so...
extern "C" char __wcpp_4_data_init_fs_root_ = '\0';
