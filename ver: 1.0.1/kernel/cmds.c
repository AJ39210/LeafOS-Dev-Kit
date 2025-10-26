// kernel/cmds.c
#include "../include/cmds.h"
#include "../include/vga.h"
#include "../include/shell.h"

void cmd_help(const char* args) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("Available Commands:\n");
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    
    vga_print("  help");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("   - Show this help message\n");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_print("  about");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("  - Show information about this OS\n");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_print("  echo");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("   - Echo back text (usage: echo <text>)\n");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_print("  clear");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("  - Clear the screen\n\n");
}

void cmd_about(const char* args) {
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga_print("\nLeafOS v1.0.1\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("===========================================\n");
    vga_print("A simple operating system built from scratch\n\n");
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("Features:\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("  * GRUB Bootloader with custom background\n");
    vga_print("  * VGA Text Mode Driver (80x25)\n");
    vga_print("  * PS/2 Keyboard Driver\n");
    vga_print("  * Simple Command Shell\n");
    vga_print("  * kernel panic added\n");
    vga_print("  * Dual kernel support (Normal & Debug)\n\n");
    
    vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    vga_print("Built with C and Assembly\n");
    vga_print("Architecture: x86 (32-bit)\n\n");
}

void cmd_echo(const char* args) {
    if (*args) {
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga_print(args);
        vga_print("\n");
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Usage: echo <text>\n");
    }
}

void cmd_clear(const char* args) {
    vga_clear();
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("Custom OS Shell\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}