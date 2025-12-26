// src/drivers/vga.c - VGA text mode driver

#define DRIVER_VERSION "1.0.0"
#define DRIVER_TYPE "Hardware Driver"
#define DRIVER_NAME "VGA Text Mode Driver"

#include "vga.h"
#include <stdint.h>
#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

static uint16_t* vga_buffer;
static size_t vga_row;
static size_t vga_column;
static uint8_t vga_color;

// Create VGA color attribute
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

// Create VGA entry with character and color
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t)uc | (uint16_t)color << 8;
}

// Initialize VGA driver
void vga_init(void) {
    vga_buffer = (uint16_t*)VGA_MEMORY;
    vga_row = 0;
    vga_column = 0;
    vga_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

// Set text color
void vga_setcolor(uint8_t color) {
    vga_color = color;
}

// Put entry at position
void vga_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    vga_buffer[index] = vga_entry(c, color);
}

// Scroll screen up
void vga_scroll(void) {
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            size_t index = y * VGA_WIDTH + x;
            size_t next_index = (y + 1) * VGA_WIDTH + x;
            vga_buffer[index] = vga_buffer[next_index];
        }
    }
    
    // Clear last line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        vga_buffer[index] = vga_entry(' ', vga_color);
    }
}

// Put character
void vga_putchar(char c) {
    if (c == '\n') {
        vga_column = 0;
        if (++vga_row == VGA_HEIGHT) {
            vga_row = VGA_HEIGHT - 1;
            vga_scroll();
        }
        return;
    }
    
    if (c == '\b') {
        if (vga_column > 0) {
            vga_column--;
            vga_putentryat(' ', vga_color, vga_column, vga_row);
        }
        return;
    }
    
    vga_putentryat(c, vga_color, vga_column, vga_row);
    
    if (++vga_column == VGA_WIDTH) {
        vga_column = 0;
        if (++vga_row == VGA_HEIGHT) {
            vga_row = VGA_HEIGHT - 1;
            vga_scroll();
        }
    }
}

// Print string
void vga_print(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {
        vga_putchar(data[i]);
    }
}

// Print hex number
void vga_print_hex(uint32_t n) {
    char hex[] = "00000000";
    for (int i = 7; i >= 0; i--) {
        uint8_t digit = n & 0xF;
        hex[i] = digit < 10 ? '0' + digit : 'A' + digit - 10;
        n >>= 4;
    }
    vga_print(hex);
}

// Clear screen
void vga_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', vga_color);
        }
    }
    vga_row = 0;
    vga_column = 0;
}