#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Keyboard initialization
void keyboard_init(void);

// Get a character from keyboard (blocking)
char keyboard_getchar(void);

// Check if key is available
int keyboard_available(void);

// Read scancode
uint8_t keyboard_read_scancode(void);

#endif