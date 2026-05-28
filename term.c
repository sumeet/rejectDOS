static void putc(char c) {
    __asm {
        mov ah, 0x0E
        mov al, c
        mov bh, 0x00
        int 0x10
    }
}

static void backspace() {
    putc(8);    // Backspace
    putc(' ');  // Overwrite
    putc(8);    // Move back again
}

static char getc(void) {
    char c;
    __asm {
        mov ah, 0x00
        int 0x16
        mov c, al
    }
    return c;
}

static void print(char *s) {
    char c;
    while ((c = *s++) != 0) putc(c);
}

static void printnum(unsigned int n) {
    char buf[16] = {0};
    char *p = buf + 15;
    do {
        *--p = '0' + n % 10;
        n /= 10;
    } while (n);
    print(p);
    print("\r\n");
}
