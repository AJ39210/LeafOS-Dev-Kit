// kernel/panic.c
#include "../include/panic.h"
#include "../include/vga.h"

static void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void delay(uint32_t cycles) {
    for (volatile uint32_t i = 0; i < cycles; i++);
}

static void reboot(void) {
    // Try keyboard controller reboot
    uint8_t temp;
    asm volatile("cli"); // Disable interrupts
    
    // Wait for keyboard controller
    do {
        asm volatile("inb $0x64, %0" : "=a"(temp));
    } while (temp & 0x02);
    
    // Send reboot command
    outb(0x64, 0xFE);
    
    // If that didn't work, triple fault
    asm volatile("int $0x00");
    
    // Halt if all else fails
    while(1) {
        asm volatile("hlt");
    }
}

void panic_init(void) {
    // Initialize boot counter on first boot
    if (*BOOT_MAGIC_ADDR != BOOT_MAGIC) {
        *BOOT_MAGIC_ADDR = BOOT_MAGIC;
        *BOOT_COUNT_ADDR = 0;
    }
}

void panic_check_bootloop(void) {
    if (*BOOT_MAGIC_ADDR == BOOT_MAGIC) {
        (*BOOT_COUNT_ADDR)++;
        
        if (*BOOT_COUNT_ADDR >= MAX_BOOT_ATTEMPTS) {
            kernel_panic("Boot loop detected (Too many reboots)");
        }
    } else {
        *BOOT_MAGIC_ADDR = BOOT_MAGIC;
        *BOOT_COUNT_ADDR = 1;
    }
}

void panic_reset_boot_counter(void) {
    *BOOT_COUNT_ADDR = 0;
}

void kernel_panic(const char* reason) {
    // Clear screen and set blue background
    uint16_t* vga = (uint16_t*)0xB8000;
    uint8_t panic_color = (VGA_COLOR_BLUE << 4) | VGA_COLOR_WHITE;
    
    // Fill entire screen with blue
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = ' ' | (panic_color << 8);
    }
    
    // Draw panic message centered
    int start_y = 8;
    int x, y;
    
    // Title
    const char* title = "LeafOS Kernel Panic";
    x = (VGA_WIDTH - 19) / 2;
    y = start_y;
    for (int i = 0; title[i]; i++) {
        vga[y * VGA_WIDTH + x + i] = title[i] | (panic_color << 8);
    }
    
    // Separator
    y += 2;
    for (int i = 10; i < VGA_WIDTH - 10; i++) {
        vga[y * VGA_WIDTH + i] = '=' | (panic_color << 8);
    }
    
    // Main message
    y += 2;
    const char* msg1 = "Your computer ran into a problem.";
    x = (VGA_WIDTH - 34) / 2;
    for (int i = 0; msg1[i]; i++) {
        vga[y * VGA_WIDTH + x + i] = msg1[i] | (panic_color << 8);
    }
    
    y++;
    const char* msg2 = "We will reboot it for you...";
    x = (VGA_WIDTH - 29) / 2;
    for (int i = 0; msg2[i]; i++) {
        vga[y * VGA_WIDTH + x + i] = msg2[i] | (panic_color << 8);
    }
    
    // Reason
    y += 3;
    const char* reason_label = "Reason of stopping:";
    x = (VGA_WIDTH - 19) / 2;
    for (int i = 0; reason_label[i]; i++) {
        vga[y * VGA_WIDTH + x + i] = reason_label[i] | (panic_color << 8);
    }
    
    y++;
    // Calculate reason length
    int reason_len = 0;
    while (reason[reason_len]) reason_len++;
    x = (VGA_WIDTH - reason_len) / 2;
    for (int i = 0; reason[i]; i++) {
        vga[y * VGA_WIDTH + x + i] = reason[i] | (panic_color << 8);
    }
    
    // Instructions
    y += 3;
    const char* inst1 = "Please enter the debug mode through the";
    x = (VGA_WIDTH - 40) / 2;
    for (int i = 0; inst1[i]; i++) {
        vga[y * VGA_WIDTH + x + i] = inst1[i] | (panic_color << 8);
    }
    
    y++;
    const char* inst2 = "GRUB menu after reboot (wait 60 seconds)";
    x = (VGA_WIDTH - 40) / 2;
    for (int i = 0; inst2[i]; i++) {
        vga[y * VGA_WIDTH + x + i] = inst2[i] | (panic_color << 8);
    }
    
    // Countdown
    y += 3;
    x = (VGA_WIDTH - 26) / 2;
    
    for (int countdown = 60; countdown >= 0; countdown--) {
        // Clear countdown line
        for (int i = 0; i < VGA_WIDTH; i++) {
            vga[y * VGA_WIDTH + i] = ' ' | (panic_color << 8);
        }
        
        // Show countdown
        if (countdown > 0) {
            char count_str[32] = "Rebooting in ";
            int idx = 13;
            
            // Convert number to string (handle up to 99)
            if (countdown >= 10) {
                count_str[idx++] = '0' + (countdown / 10);
                count_str[idx++] = '0' + (countdown % 10);
            } else {
                count_str[idx++] = '0' + countdown;
            }
            count_str[idx++] = ' ';
            count_str[idx++] = 's';
            count_str[idx++] = 'e';
            count_str[idx++] = 'c';
            count_str[idx++] = 'o';
            count_str[idx++] = 'n';
            count_str[idx++] = 'd';
            count_str[idx++] = 's';
            count_str[idx++] = '.';
            count_str[idx++] = '.';
            count_str[idx++] = '.';
            count_str[idx] = '\0';
            
            x = (VGA_WIDTH - idx) / 2;
            for (int i = 0; i < idx; i++) {
                vga[y * VGA_WIDTH + x + i] = count_str[i] | (panic_color << 8);
            }
        } else {
            const char* reboot_now = "Rebooting now...";
            x = (VGA_WIDTH - 16) / 2;
            for (int i = 0; i < 16; i++) {
                vga[y * VGA_WIDTH + x + i] = reboot_now[i] | (panic_color << 8);
            }
        }
        
        // Wait 1 second (approximate - increased delay for more accurate timing)
        delay(50000000);
    }
    
    // Reboot
    reboot();
}