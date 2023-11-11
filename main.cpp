// GOD TUTORIAL:
// https://sebastian.graphics/blog/16-bit-tiny-model-standalone-c-with-open-watcom.html

#include "term.cpp"

#define INPUT_SIZE 512

char input[INPUT_SIZE] = {0};
short input_len = 0;

void eval() {
    print("evaluating: ");
    println(input);
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
char __wcpp_4_data_init_fs_root_ = '\0';
