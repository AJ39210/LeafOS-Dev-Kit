// kernel/kernel.c
#include "../include/vga.h"
#include "../include/keyboard.h"
#include "../include/shell.h"
#include "../include/panic.h"

// Set to 1 to test kernel panic, 0 for normal operation
#define KERNEL_PANIC_TEST 1

void kernel_main(void) {
    vga_init();
    
    // Initialize panic system and check for boot loops
    panic_init();
    panic_check_bootloop();
    
    // Test kernel panic if enabled
    if (KERNEL_PANIC_TEST) {
        kernel_panic("Developer initilized kernel panic throw kernel.c script");
    }
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print("===========================================\n");
    vga_print("        LeafOS - Production Kernel v1.0.1  \n");
    vga_print("===========================================\n\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("Initializing system components...\n");
    
    keyboard_init();
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("System ready!\n");
    
    // Reset boot counter on successful boot
    panic_reset_boot_counter();
    
    // Start shell
    shell_init();
    shell_run();
}