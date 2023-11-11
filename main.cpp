// GOD TUTORIAL:
// https://sebastian.graphics/blog/16-bit-tiny-model-standalone-c-with-open-watcom.html

void putc(char c) {
    // AH = 0x0E (Teletype output)
    // AL = character to print
    // BH = page number (usually 0)
    // BL = foreground color (not used in text mode)
    __asm {
        mov ah, 0x0E
        mov al, c
        mov bh, 0x00
        int 0x10
    }
}

void backspace() {
    unsigned char row, col;

    // Get current cursor position
    __asm {
        mov ah, 0x03  // Function 0x03: Read Cursor Position
        mov bh, 0x00  // Page number
        int 0x10  // BIOS Video Service
        mov row, dh  // Row
        mov col, dl  // Column
    }

    if (col > 0) {
        col--;  // Move back one column, if not at start of line
    } else if (row > 0) {
        row--;     // Move up one row, if not at first row
        col = 79;  // Position at end of the previous line
    } else {
        // At the top-left corner of the screen, do nothing
        return;
    }

    // Set cursor to new position
    __asm {
        mov ah, 0x02  // Function 0x02: Set Cursor Position
        mov bh, 0x00  // Page number
        mov dh, row  // Row
        mov dl, col  // Column
        int 0x10  // BIOS Video Service
    }

    putc(' ');

    // Move the cursor back to the position of the erased character
    __asm {
        mov ah, 0x02  // Function 0x02: Set Cursor Position
        mov bh, 0x00  // Page number
        mov dh, row  // Row
        mov dl, col  // Column
        int 0x10  // BIOS Video Service
    }
}

char getc(void) {
    char c;
    __asm {
        mov ah, 0x00  // Function 0x00: Read Character from Keyboard
        int 0x16  // BIOS Keyboard Service
        mov c, al
    }
    return c;
}

void print(char *s) {
    while (char c = *s++) putc(c);
}

void println(char *s) {
    print(s);
    print("\r\n");
}

void printnum(unsigned int n) {
    char buf[16] = {0};
    char *p = buf + 15;
    do {
        *--p = '0' + n % 10;
        n /= 10;
    } while (n);
    print(p);
    print("\r\n");
}

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
        while (!cmd_done) {
            char c = getc();
            switch (c) {
                case '\r':
                    cmd_done = 1;
                    print("\r\n");
                    eval();
                    break;
                case 127:  // backspace
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
