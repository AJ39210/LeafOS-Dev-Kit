// src/kernel/syscalls.c - System call implementations

#define DRIVER_VERSION "1.0.0"
#define DRIVER_TYPE "System Driver"
#define DRIVER_NAME "Syscall Handler"

#include <stdint.h>

// Forward declarations
void vga_print(const char* data);
void vga_putchar(char c);

// Syscall numbers
#define SYS_WRITE  1
#define SYS_EXIT   2

// Simple syscall handler
void syscall_handler(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    (void)arg2;
    (void)arg3;
    
    switch(syscall_num) {
        case SYS_WRITE:
            // arg1 = string pointer
            vga_print((const char*)arg1);
            break;
            
        case SYS_EXIT:
            // arg1 = exit code
            vga_print("\n[PROCESS] Exited with code: ");
            // Simple number print
            if (arg1 == 0) vga_putchar('0');
            else vga_putchar('0' + (arg1 % 10));
            vga_putchar('\n');
            break;
            
        default:
            vga_print("[SYSCALL] Unknown syscall: ");
            vga_putchar('0' + (syscall_num % 10));
            vga_putchar('\n');
            break;
    }
}

// Exposed syscall interface for user programs
void sys_write(const char* str) {
    syscall_handler(SYS_WRITE, (uint32_t)str, 0, 0);
}

void sys_exit(int code) {
    syscall_handler(SYS_EXIT, (uint32_t)code, 0, 0);
}