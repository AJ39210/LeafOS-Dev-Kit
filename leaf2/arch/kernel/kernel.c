#include "vga.h"
#include "keyboard.h"
#include "memory.h"
#include "shell.h"
#include "idt.h"
#include "initrd.h"  // ← Still included for manual use

void kernel_main(void) {
    // Initialize VGA driver
    vga_init();
    idt_init();  // Load IDT before enabling interrupts

    // Print boot banner
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write("========================================\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("         LeafOS v9.5\n");
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
    vga_write("Memory Manager initialized\n");

    vga_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga_write("[INFO] ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Initrd is available. Type 'run initrd' to load it.\n\n");

    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("System ready! Type 'help' for available commands.\n\n");

    // Start shell
    shell_init();
    shell_run();

    // Halt if shell exits
    __asm__ __volatile__("cli; hlt");
    for (;;);
}
