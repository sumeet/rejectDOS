#include "term.c"

#define INPUT_SIZE 32

char input[INPUT_SIZE] = {0};
short input_len = 0;

void eval() {
    print("evaluating: ");
    println(input);
}

int main(void) {
    int cmd_done;
    char c;
    while (1) {
        print("> ");
        cmd_done = 0;
        input_len = 0;
        while (!cmd_done) {
            c = getc();
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

char __wcpp_4_data_init_fs_root_ = '\0';
