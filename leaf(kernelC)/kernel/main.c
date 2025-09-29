/* kernel/main.c  -- simple 32-bit kernel that writes to VGA text buffer */
void kmain(void) {
    unsigned short *video = (unsigned short*)0xB8000;
    const char *msg = "Hello 32-bit Kernel! Protected mode works.";
    for (int i = 0; msg[i]; ++i) {
        video[i] = (0x07 << 8) | msg[i];  /* color 0x07 + ASCII */
    }
    /* hang */
    for (;;) {}
}
