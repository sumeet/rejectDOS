// A simple emulation of the terminal functionality from rejectDOS
// This demonstrates the core concepts of the project

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INPUT_SIZE 512

// Simulate BIOS-like terminal functions
void putc_sim(char c) {
    putchar(c);
    fflush(stdout);
}

void print_sim(const char* s) {
    printf("%s", s);
    fflush(stdout);
}

void println_sim(const char* s) {
    printf("%s\n", s);
    fflush(stdout);
}

// Simulate backspace functionality (simplified)
void backspace_sim() {
    printf("\b \b");
    fflush(stdout);
}

// Simulate getting a character (simplified)
char getc_sim() {
    char c;
    scanf("%c", &c);
    return c;
}

void eval_sim(const char* input) {
    printf("evaluating: %s\n", input);
}

int main() {
    char input[INPUT_SIZE] = {0};
    int input_len = 0;
    
    printf("=== rejectDOS simulated shell ===\n");
    printf("This demonstrates what the actual rejectDOS project would do\n");
    printf("It's a 16-bit DOS terminal shell with command input\n");
    printf("\n");
    
    while (1) {
        printf("> ");
        fflush(stdout);
        
        int cmd_done = 0;
        input_len = 0;
        
        while (!cmd_done) {
            char c = getc_sim();
            
            switch (c) {
                case '\r':  // Enter key
                    printf("\n");
                    eval_sim(input);
                    memset(input, 0, INPUT_SIZE);
                    input_len = 0;
                    cmd_done = 1;
                    break;
                    
                case 127:  // Backspace
                    if (input_len > 0) {
                        input[--input_len] = '\0';
                        backspace_sim();
                    }
                    break;
                    
                default:
                    if (input_len < INPUT_SIZE - 1) {
                        input[input_len++] = c;
                        putc_sim(c);
                    }
                    break;
            }
        }
    }
    
    return 0;
}