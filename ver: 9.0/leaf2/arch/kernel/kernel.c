#include "vga.h"
#include "keyboard.h"
#include "memory.h"
#include "shell.h"
#include "types.h"
#include "idt.h"

void kernel_main(void) {
    // Initialize VGA driver
    vga_init();
    
    // Print boot banner
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write("========================================\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("         LeafOS v9.0\n");
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write("========================================\n\n");
    
    // Initialize drivers and show status
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("[OK] ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("VGA Driver initialized\n");
    
    keyboard_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("[OK] ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Keyboard Driver initialized\n");
    
    memory_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("[OK] ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Memory Manager initialized\n\n");

    idt_init();
    vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    vga_write("[WARN!] ");    //Important Needed driver so its warning the user to be cautios if the idt is not loaded!
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("IDT (Interrupt Descriptor Table) initialized\n\n");

    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("System ready! Type 'help' for available commands.\n\n");
    
    // Initialize and run shell
    shell_init();
    shell_run();
    
    // If shell exits (shouldn't happen), halt forever
    __asm__ __volatile__("cli; hlt");
    for(;;);
}