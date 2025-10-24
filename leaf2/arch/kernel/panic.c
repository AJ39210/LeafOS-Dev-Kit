#include "panic.h"
#include "vga.h"
#include "types.h"

// Embedded config - will be set at compile time
// 1 = test mode ON, 0 = test mode OFF
static const int TEST_MODE = 0;  // Change this to 1 to enable test mode

int panic_is_test_mode(void) {
    return TEST_MODE;
}

void kernel_panic(const char* reason) {
    // Disable interrupts - we're in panic mode
    __asm__ __volatile__("cli");
    
    // Clear screen with blue background
    uint16_t* vga_mem = (uint16_t*)0xB8000;
    uint16_t blue_space = 0x1F20;  // Blue background, white text, space
    
    for (int i = 0; i < 80 * 25; i++) {
        vga_mem[i] = blue_space;
    }
    
    // Reset VGA to print at top
    vga_init();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    
    // Print panic header
    vga_write("\n\n");
    vga_write("  ========================================\n");
    vga_write("           KERNEL PANIC!\n");
    vga_write("  ========================================\n\n");
    
    vga_write("  Critical proces died. Reboot your PC and\n");
    vga_write("  try again!\n\n");
    vga_write("  If the problem persists, ask for support\n");
    vga_write("  in our support channel.\n\n");
    
    // Print reason
    vga_write("  Reason: ");
    vga_write(reason);
    vga_write("\n\n");
    
    vga_write("  ========================================\n");
    
    // Halt forever
    while(1) {
        __asm__ __volatile__("hlt");
    }
}