void main(void) {
    char far* x = (char far*) 0xb8000000;
    x[0] = 'A';
    x[1] = 0x70;
    return;
}
