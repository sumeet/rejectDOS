#include <stdio.h>
#include <string.h>

// Simple simulation of what the rejectDOS terminal would do
void putc(char c) {
    putchar(c);
}

void print(const char* s) {
    printf("%s", s);
}

void println(const char* s) {
    printf("%s\n", s);
}

char getc() {
    char c;
    scanf("%c", &c);
    return c;
}

void backspace() {
    print("\b \b");  // Simple backspace simulation
}

void eval(const char* input) {
    print("evaluating: ");
    println(input);
}

int main() {
    char input[512] = {0};
    int input_len = 0;
    int cmd_done = 0;
    
    printf("rejectDOS shell ready (simulated)\n");
    printf("> ");
    
    while (1) {
        char c = getc();
        switch (c) {
            case '\r':
                printf("\n");
                eval(input);
                memset(input, 0, sizeof(input));
                input_len = 0;
                printf("> ");
                break;
            case 127:  // Backspace
                if (input_len > 0) {
                    input[--input_len] = '\0';
                    backspace();
                }
                break;
            default:
                if (input_len < 511) {
                    input[input_len++] = c;
                    putc(c);
                }
                break;
        }
    }
    
    return 0;
}