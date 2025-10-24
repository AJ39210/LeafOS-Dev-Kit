#include "vga.h"

static uint16_t* vga_buffer;
static uint32_t vga_index;
static uint8_t vga_fg_color;
static uint8_t vga_bg_color;

static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

void vga_init(void) {
    vga_buffer = (uint16_t*) VGA_MEMORY;
    vga_index = 0;
    vga_fg_color = VGA_COLOR_LIGHT_GREY;
    vga_bg_color = VGA_COLOR_BLACK;
    vga_clear();
}

void vga_clear(void) {
    uint8_t color = vga_entry_color(vga_fg_color, vga_bg_color);
    uint16_t blank = vga_entry(' ', color);
    
    for (uint32_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = blank;
    }
    vga_index = 0;
}

void vga_set_color(uint8_t fg, uint8_t bg) {
    vga_fg_color = fg;
    vga_bg_color = bg;
}

void vga_newline(void) {
    uint32_t row = vga_index / VGA_WIDTH;
    vga_index = (row + 1) * VGA_WIDTH;
    
    if (vga_index >= VGA_WIDTH * VGA_HEIGHT) {
        // Scroll up
        for (uint32_t i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) {
            vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
        }
        
        // Clear last line
        uint8_t color = vga_entry_color(vga_fg_color, vga_bg_color);
        uint16_t blank = vga_entry(' ', color);
        for (uint32_t i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++) {
            vga_buffer[i] = blank;
        }
        
        vga_index = VGA_WIDTH * (VGA_HEIGHT - 1);
    }
}

void vga_putchar(char c) {
    uint8_t color = vga_entry_color(vga_fg_color, vga_bg_color);
    
    if (c == '\n') {
        vga_newline();
        return;
    }
    
    if (c == '\t') {
        for (int i = 0; i < 4; i++) {
            vga_putchar(' ');
        }
        return;
    }
    
    if (c == '\r') {
        vga_index = (vga_index / VGA_WIDTH) * VGA_WIDTH;
        return;
    }
    
    vga_buffer[vga_index] = vga_entry(c, color);
    vga_index++;
    
    if (vga_index >= VGA_WIDTH * VGA_HEIGHT) {
        vga_newline();
    }
}

void vga_write(const char* str) {
    for (uint32_t i = 0; str[i] != '\0'; i++) {
        vga_putchar(str[i]);
    }
}

void vga_write_hex(uint32_t num) {
    char hex_chars[] = "0123456789ABCDEF";
    vga_write("0x");
    
    int started = 0;
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t digit = (num >> i) & 0xF;
        if (digit != 0 || started || i == 0) {
            vga_putchar(hex_chars[digit]);
            started = 1;
        }
    }
}

void vga_write_dec(uint32_t num) {
    if (num == 0) {
        vga_putchar('0');
        return;
    }
    
    char buffer[12];
    int i = 0;
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    for (int j = i - 1; j >= 0; j--) {
        vga_putchar(buffer[j]);
    }
}
    