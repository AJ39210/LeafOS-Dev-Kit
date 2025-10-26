// kernel/kernel.c
#include "../include/vga.h"
#include "../include/keyboard.h"
#include "../include/shell.h"
#include "../include/panic.h"
#include "../include/fat32.h"

// Set to 1 to test kernel panic, 0 for normal operation
#define KERNEL_PANIC_TEST 0

void kernel_main(void) {
    vga_init();
    
    // Initialize panic system and check for boot loops
    panic_init();
    panic_check_bootloop();
    
    // Test kernel panic if enabled
    if (KERNEL_PANIC_TEST) {
        kernel_panic("Testing");
    }
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print("===========================================\n");
    vga_print("        LeafOS - Production Kernel v1.0.5  \n");
    vga_print("===========================================\n\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("Initializing system components...\n");
    
    keyboard_init();
    
    // Initialize FAT32 filesystem
    fat32_init();
    fat32_mount();
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("System ready!\n");
    
    // Reset boot counter on successful boot
    panic_reset_boot_counter();
    
    // Start shell
    shell_init();
    shell_run();
}