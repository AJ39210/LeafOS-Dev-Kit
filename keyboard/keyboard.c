// kernel/keyboard.c
#include "../include/keyboard.h"
#include "../include/vga.h"

static const char scancode_to_ascii[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' '
};

static uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void keyboard_init(void) {
    vga_print(".\n");
}

static uint8_t last_scancode = 0;

char keyboard_getchar(void) {
    uint8_t scancode = inb(0x60);
    
    // Ignore if same as last scancode (debounce)
    if (scancode == last_scancode) {
        return 0;
    }
    
    last_scancode = scancode;
    
    if (scancode & 0x80) {
        return 0; // Key release
    }
    
    if (scancode < sizeof(scancode_to_ascii)) {
        return scancode_to_ascii[scancode];
    }
    
    return 0;
}

void keyboard_wait_key(void) {
    while (1) {
        char c = keyboard_getchar();
        if (c != 0) {
            vga_putchar(c);
            return;
        }
        for (volatile int i = 0; i < 10000; i++);
    }
}
