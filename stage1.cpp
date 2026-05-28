// GOD TUTORIAL:
// https://sebastian.graphics/blog/16-bit-tiny-model-standalone-c-with-open-watcom.html

void print_char_with_color_at(char c, unsigned char color, unsigned char row, unsigned char col) {
    // Set cursor position
    __asm {
        mov ah, 0x02   ; BIOS function to set cursor position
        mov dh, row    ; Row
        mov dl, col    ; Column
        int 0x10       ; Call BIOS
    }

    // Print character
    __asm {
        mov ah, 0x09   ; BIOS function to write character with attribute
        mov al, c      ; Character to write
        mov bl, color  ; Color attribute
        mov bh, 0x00   ; Page number
        mov cx, 1      ; Number of times to write character
        int 0x10       ; Call BIOS
    }
}

char *text = "\n                                    \n                .-.             /   \n   ).--..-.     `-'.-.  .-. ---/--- \n  /   ./.-'_    /./.-'_(      /     \n /    (__.'(   / (__.'  `---'/      \n            `-'                     \n";

void print_gradient_text() {
  // orange, purple, grey
  // looks for \n for queues on when to move to next line
  // optimize for small code size
  unsigned char color = 0x0E;
  unsigned char row, col;

    __asm {
        mov ah, 0x03   ; BIOS function to get cursor position
        int 0x10       ; Call BIOS
        mov row, dh      ; Row
        mov col, dl      ; Column
    }

  char *c = text;
  while (*c != '\0') {
    if (*c == '\n') {
      row++;
      col = 0;
    } else {
      print_char_with_color_at(*c, color, row, col);
      col++;
    }
    color++;
    if (color > 0x0F) {
      color = 0x0E;
    }
    c++;
  }
  print_char_with_color_at(' ', color, row+1, 0);
}

int main() {
    print_gradient_text();
}


// for some reason the linker keeps looking for this, so...
extern "C" char __wcpp_4_data_init_fs_root_ = '\0';
