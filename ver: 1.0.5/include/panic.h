// include/panic.h
#ifndef PANIC_H
#define PANIC_H

#include <stdint.h>

#define PANIC_REASON_MAX_LEN 64

// Boot loop detection
#define BOOT_MAGIC 0xDEADBEEF
#define BOOT_COUNT_ADDR ((uint32_t*)0x8000)  // Safe memory location
#define BOOT_MAGIC_ADDR ((uint32_t*)0x8004)
#define MAX_BOOT_ATTEMPTS 3

void panic_init(void);
void kernel_panic(const char* reason);
void panic_check_bootloop(void);
void panic_reset_boot_counter(void);

#endif