void kmain(void) {
    volatile unsigned short* vga = (unsigned short*)0xB8000;
    vga[0] = (0x0F << 8) | 'K'; // prints 'K' white
    vga[1] = (0x0F << 8) | 'E'; // prints 'E' white
    for (;;) __asm__ __volatile__("hlt");
}
