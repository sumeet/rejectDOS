// GOD TUTORIAL:
// https://sebastian.graphics/blog/16-bit-tiny-model-standalone-c-with-open-watcom.html

#include "term.cpp"

#define INPUT_SIZE 512
#define TOKENS_SIZE 128

char input[INPUT_SIZE];
short input_len;

struct StringView {
  char *data;
  unsigned int len;
};

enum TokenType {
  TokenNumber,
  TokenOperator,
};

struct Token {
  TokenType type;
  StringView value;
};

Token tokens[TOKENS_SIZE];
short tokens_len;

void tokenize(char *input) {
  tokens_len = 0;
  char *c = input;
  while (*c) {
    if (*c >= '0' && *c <= '9') {
      char *start = c;
      //while (*c >= '0' && *c <= '9') c++;
      Token token = tokens[tokens_len++];
      token.type = TokenNumber;
      token.value.data = start;
      token.value.len = c - start;
    } else if (*c == '+') {
    //} else if (*c == '+' || *c == '-' || *c == '*' || *c == '/') {
      Token token = tokens[tokens_len++];
      token.type = TokenOperator;
      token.value.data = c;
    } else if (*c == ' ') {
      // do nothing
    } else {
      putc(*c);
      return;
    }
    c++;
  }
}

void eval() {
    print("evaluating: ");
    println(input);
    tokenize(input);
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
