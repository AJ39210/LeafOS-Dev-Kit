// kernel/cmds.c
#include "../include/cmds.h"
#include "../include/vga.h"
#include "../include/shell.h"
#include "../include/fat32.h"

void cmd_help(const char* args) {
    (void)args; // Suppress unused warning
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
    vga_print("  - Clear the screen\n");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_print("  ls");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("     - List files in filesystem\n");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_print("  cat");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("    - Display file contents (usage: cat <filename>)\n");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_print("  touch");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("  - Create a test file (usage: touch <filename>)\n\n");
}

void cmd_about(const char* args) {
    (void)args; // Suppress unused warning
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga_print("\nLeafOS v1.0.5\n");
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
    vga_print("  * Dual kernel support (Normal & Debug)\n");
    vga_print("  * Kernel Panic & Boot Loop Detection\n\n");
    
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
    (void)args; // Suppress unused warning
    vga_clear();
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("LeafOS Shell\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void cmd_ls(const char* args) {
    (void)args; // Suppress unused warning
    fat32_list_root();
}

void cmd_cat(const char* args) {
    if (!*args) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Usage: cat <filename>\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        return;
    }
    
    if (!fat32_file_exists(args)) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("File not found: ");
        vga_print(args);
        vga_print("\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        return;
    }
    
    uint32_t size = fat32_get_file_size(args);
    if (size == 0) {
        vga_print("(empty file)\n");
        return;
    }
    
    uint8_t buffer[1024];
    int bytes = fat32_read_file(args, buffer, sizeof(buffer) - 1);
    
    if (bytes > 0) {
        buffer[bytes] = '\0';
        vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        vga_print("\n--- ");
        vga_print(args);
        vga_print(" ---\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        
        for (int i = 0; i < bytes; i++) {
            vga_putchar(buffer[i]);
        }
        vga_print("\n");
        vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
        vga_print("--- End of file ---\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Error reading file\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    }
}

void cmd_touch(const char* args) {
    if (!*args) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Usage: touch <filename>\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        return;
    }
    
    if (fat32_file_exists(args)) {
        vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
        vga_print("File already exists: ");
        vga_print(args);
        vga_print("\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        return;
    }
    
    const char* content = "Hello from LeafOS! This is a test file.\n";
    int len = 0;
    while (content[len]) len++;
    
    int result = fat32_write_file(args, (const uint8_t*)content, len);
    
    if (result > 0) {
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print("Created file: ");
        vga_print(args);
        vga_print("\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Error creating file\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    }
}