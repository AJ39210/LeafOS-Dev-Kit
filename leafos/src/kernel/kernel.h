// src/kernel/kernel.h - Kernel header file

#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

// Port I/O functions
void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

// Interrupt management
void enable_interrupts(void);
void disable_interrupts(void);

#endif