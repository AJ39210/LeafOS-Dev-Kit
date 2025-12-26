// src/drivers/keyboard.h - Keyboard driver header

#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyboard_init(void);
void keyboard_poll(void);
char keyboard_getchar(void);

#endif