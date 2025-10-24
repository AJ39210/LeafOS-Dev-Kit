#include "vga.h"
#include "keyboard.h"
#include "memory.h"
#include "shell.h"
#include "idt.h"
#include "panic.h"
#include "types.h"

void kernel_main(void) {
    // Initialize VGA driver FIRST - we need it for panic messages
    vga_init();
    
    // Check if we're in test mode
    if (panic_is_test_mode()) {
        kernel_panic("Testing kernel panic system...");
    }
    
    // Print boot banner
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write("========================================\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("         LeafOS v1.0\n");
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write("========================================\n\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Initializing drivers...\n\n");
    
    // Initialize VGA (already done)
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("[OK] ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("VGA Driver initialized\n");
    
    // Setup IDT
    idt_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("[OK] ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("IDT initialized\n");
    
    // Initialize keyboard
    keyboard_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("[OK] ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Keyboard Driver initialized\n");
    
    // Initialize memory
    memory_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("[OK] ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Memory Manager initialized\n");
    
    // Show detected memory
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_write("      Detected ");
    vga_write_dec(memory_get_total_kb());
    vga_write(" KB RAM\n\n");
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("System ready! Type 'help' for available commands.\n\n");
    
    // Run shell - this should never return
    shell_init();
    shell_run();
    
    // If we somehow get here, infinite loop
    while(1) {
        __asm__ __volatile__("hlt");
    }
}