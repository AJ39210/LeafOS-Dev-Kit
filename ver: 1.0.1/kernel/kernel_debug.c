// kernel/kernel_debug.c
#include "../include/vga.h"
#include "../include/keyboard.h"

void kernel_main(void) {
    vga_init();
    vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    vga_print("===========================================\n");
    vga_print("    LeafOS - DEBUG Kernel v1.0.0  \n");
    vga_print("===========================================\n\n");
    
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga_print("DEBUG MODE ENABLED\n");
    vga_print("Verbose logging active\n\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("[DEBUG] Initializing VGA driver...\n");
    vga_print("[DEBUG] VGA buffer at 0xB8000\n");
    vga_print("[DEBUG] Screen size: 80x25\n\n");
    
    vga_print("[DEBUG] Initializing keyboard driver...\n");
    keyboard_init();
    vga_print("[DEBUG] Keyboard port: 0x60\n\n");
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("System ready (DEBUG). Type something:\n");
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    
    while (1) {
        char c = keyboard_getchar();
        if (c != 0) {
            vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
            vga_print("[KEY] ");
            vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
            vga_putchar(c);
            vga_putchar('\n');
            // Wait for key release to prevent repeats
            while (keyboard_getchar() != 0);
        }
        for (volatile int i = 0; i < 5000; i++);
    }
}