// main.c - 32-bit kernel
void kmain(void) {
    unsigned short *video = (unsigned short*)0xB8000;
    const char *str = "Hello 32-bit Kernel!";
    for (int i = 0; str[i] != 0; i++) {
        video[i] = (0x07 << 8) | str[i];  // color + char
    }
    while (1); // halt
}
