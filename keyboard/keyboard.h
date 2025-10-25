// include/keyboard.h
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
char keyboard_getchar(void);
void keyboard_wait_key(void);

#endif
