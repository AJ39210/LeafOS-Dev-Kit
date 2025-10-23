#include "keyboard.h"

// US QWERTY keyboard scancode to ASCII map
static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

void keyboard_init(void) {
    // Nothing special needed for basic PS/2 keyboard
}

uint8_t keyboard_read_scancode(void) {
    while (!(inb(KEYBOARD_STATUS_PORT) & 1));
    return inb(KEYBOARD_DATA_PORT);
}

int keyboard_available(void) {
    return inb(KEYBOARD_STATUS_PORT) & 1;
}

char keyboard_getchar(void) {
    uint8_t scancode;
    
    // Keep trying until we get a valid character
    while (1) {
        // Wait for data to be available
        while (!(inb(KEYBOARD_STATUS_PORT) & 1));
        
        // Read the scancode
        scancode = inb(KEYBOARD_DATA_PORT);
        
        // Only handle key press (scancodes < 0x80)
        if (scancode < 0x80 && scancode < sizeof(scancode_to_ascii)) {
            char c = scancode_to_ascii[scancode];
            if (c != 0) {
                return c;
            }
        }
    }
}