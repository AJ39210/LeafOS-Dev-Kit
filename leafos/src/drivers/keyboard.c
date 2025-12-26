// src/drivers/keyboard.c - PS/2 keyboard driver

#define DRIVER_VERSION "0.1.0"
#define DRIVER_TYPE "Peripheral Driver"
#define DRIVER_NAME "PS/2 Keyboard Driver"

#include "keyboard.h"
#include "vga.h"
#include <stdint.h>

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

extern uint8_t inb(uint16_t port);
extern void outb(uint16_t port, uint8_t value);

static int shift_pressed = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;
static int caps_lock = 0;

// US QWERTY keyboard layout - lowercase
static const char keyboard_map_lower[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, /* Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, /* Right shift */
    '*',
    0,  /* Alt */
    ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* F1 */
    0, 0, 0, 0, 0, 0, 0, 0, 0,  /* F2 - F10 */
    0,  /* Num lock */
    0,  /* Scroll Lock */
    '7',  /* Home key / Numpad 7 */
    '8',  /* Up Arrow / Numpad 8 */
    '9',  /* Page Up / Numpad 9 */
    '-',  /* Numpad - */
    '4',  /* Left Arrow / Numpad 4 */
    '5',  /* Numpad 5 */
    '6',  /* Right Arrow / Numpad 6 */
    '+',  /* Numpad + */
    '1',  /* End key / Numpad 1 */
    '2',  /* Down Arrow / Numpad 2 */
    '3',  /* Page Down / Numpad 3 */
    '0',  /* Insert Key / Numpad 0 */
    '.',  /* Delete Key / Numpad . */
    0, 0, 0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,
};

// US QWERTY keyboard layout - uppercase (with shift)
static const char keyboard_map_upper[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, /* Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, /* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0, /* Right shift */
    '*',
    0,  /* Alt */
    ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* F1 */
    0, 0, 0, 0, 0, 0, 0, 0, 0,  /* F2 - F10 */
    0,  /* Num lock */
    0,  /* Scroll Lock */
    '7',  /* Home key / Numpad 7 */
    '8',  /* Up Arrow / Numpad 8 */
    '9',  /* Page Up / Numpad 9 */
    '-',  /* Numpad - */
    '4',  /* Left Arrow / Numpad 4 */
    '5',  /* Numpad 5 */
    '6',  /* Right Arrow / Numpad 6 */
    '+',  /* Numpad + */
    '1',  /* End key / Numpad 1 */
    '2',  /* Down Arrow / Numpad 2 */
    '3',  /* Page Down / Numpad 3 */
    '0',  /* Insert Key / Numpad 0 */
    '.',  /* Delete Key / Numpad . */
    0, 0, 0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,
};

void keyboard_init(void) {
    // Initialize keyboard state
    shift_pressed = 0;
    ctrl_pressed = 0;
    alt_pressed = 0;
    caps_lock = 0;
}

void keyboard_poll(void) {
    // Check if keyboard has data
    uint8_t status = inb(KEYBOARD_STATUS_PORT);
    if (!(status & 0x01)) {
        return; // No data available
    }
    
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Handle key release (scancode with bit 7 set)
    if (scancode & 0x80) {
        scancode &= 0x7F; // Clear the release bit
        
        // Check for modifier key releases
        if (scancode == 0x2A || scancode == 0x36) { // Left or Right Shift
            shift_pressed = 0;
        } else if (scancode == 0x1D) { // Ctrl
            ctrl_pressed = 0;
        } else if (scancode == 0x38) { // Alt
            alt_pressed = 0;
        }
        return;
    }
    
    // Handle key press
    if (scancode == 0x2A || scancode == 0x36) { // Left or Right Shift
        shift_pressed = 1;
        return;
    } else if (scancode == 0x1D) { // Ctrl
        ctrl_pressed = 1;
        return;
    } else if (scancode == 0x38) { // Alt
        alt_pressed = 1;
        return;
    } else if (scancode == 0x3A) { // Caps Lock
        caps_lock = !caps_lock;
        return;
    }
    
    // Get the character based on shift/caps state
    char c;
    if (shift_pressed || caps_lock) {
        if (caps_lock && !shift_pressed && scancode >= 16 && scancode <= 50) {
            c = keyboard_map_upper[scancode];
        } else if (shift_pressed && caps_lock && scancode >= 16 && scancode <= 50) {
            c = keyboard_map_lower[scancode]; // Shift + Caps = lowercase letters
        } else if (shift_pressed) {
            c = keyboard_map_upper[scancode];
        } else {
            c = keyboard_map_lower[scancode];
        }
    } else {
        c = keyboard_map_lower[scancode];
    }
    
    if (c != 0) {
        vga_putchar(c);
    }
}

char keyboard_getchar(void) {
    uint8_t scancode;
    
    while ((inb(KEYBOARD_STATUS_PORT) & 1) == 0);
    scancode = inb(KEYBOARD_DATA_PORT);
    
    if (scancode & 0x80) {
        return 0;  // Key release
    }
    
    if (shift_pressed || caps_lock) {
        return keyboard_map_upper[scancode];
    }
    
    return keyboard_map_lower[scancode];
}
