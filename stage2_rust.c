#include "term.c"

#define INPUT_SIZE 64

char input[INPUT_SIZE];
short input_len;

// Native 16-bit Real Mode Evaluator in C
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
