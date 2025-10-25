// kernel/kernel.c
#include "../include/vga.h"
#include "../include/keyboard.h"
#include "../include/shell.h"

void kernel_main(void) {
    vga_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print("===========================================\n");
    vga_print("              LeafOS - v1.0.0              \n");
    vga_print("===========================================\n\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("Initializing system components...\n");

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("[ ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print("OK");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print(" ]");
    vga_set_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK);
    vga_print(" VGA - Driver\n");

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("[ ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print("OK");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print(" ]");
    vga_set_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK);
    vga_print(" Keyboard(ps2) - Driver\n");

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("[ ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print("OK");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print(" ]");
    vga_set_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK);
    vga_print(" Shell - app data \n");
    
    keyboard_init();
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("Loading...\n");
    
    // Start shell
    shell_init();
    shell_run();
}