#include "commands.h"
#include "vga.h"
#include "memory.h"

void command_help(void) {
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("Available commands:\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("  help    - Show this help message\n");
    vga_write("  clear   - Clear the screen\n");
    vga_write("  bitmap  - Show memory bitmap information\n");
    vga_write("  about   - About this OS\n");
}

void command_clear(void) {
    vga_clear();
}

void command_bitmap(void) {
    memory_info_t* mem = memory_get_info();
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write("=== Memory Information ===\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Total Memory:  ");
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga_write_dec(memory_get_total_kb());
    vga_write(" KB (");
    vga_write_dec(mem->total_blocks);
    vga_write(" blocks)\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Used Memory:   ");
    vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    vga_write_dec(memory_get_used_kb());
    vga_write(" KB (");
    vga_write_dec(mem->used_blocks);
    vga_write(" blocks)\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Free Memory:   ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_dec(memory_get_free_kb());
    vga_write(" KB (");
    vga_write_dec(mem->free_blocks);
    vga_write(" blocks)\n");
    
    // Calculate percentage
    uint32_t used_percent = (mem->used_blocks * 100) / mem->total_blocks;
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Usage:         ");
    vga_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga_write_dec(used_percent);
    vga_write("%\n");
}

void command_about(void) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write("MyOS v1.0\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("A simple operating system built from scratch!\n");
    vga_write("Features:\n");
    vga_write("  - Custom bootloader\n");
    vga_write("  - VGA text mode driver\n");
    vga_write("  - PS/2 keyboard driver\n");
    vga_write("  - Memory management with bitmap\n");
    vga_write("  - Simple shell interface\n");
    vga_write("\n");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("Made with <3 and lots of coffee!\n");
}