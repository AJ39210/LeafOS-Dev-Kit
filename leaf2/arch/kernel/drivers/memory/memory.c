#include "memory.h"
#include "vga.h"

static memory_info_t mem_info;

// Detect usable memory by probing writable space from 1MB upward
static uint32_t detect_memory(void) {
    volatile uint32_t* ptr;
    uint32_t total_kb = 0;

    // Scan from 1MB to 64MB in 4KB steps
    for (uint32_t addr = 0x100000; addr < 0x4000000; addr += 0x1000) {
        ptr = (uint32_t*)addr;
        uint32_t old = *ptr;
        *ptr = 0x55AA55AA;

        if (*ptr != 0x55AA55AA) {
            break;  // write failed → end of usable memory
        }

        *ptr = old;  // restore original value
        total_kb += 4;  // 4KB step
    }

    // Add base memory (below 1MB)
    total_kb += 1024;

    return total_kb;
}

void memory_init(void) {
    uint32_t total_memory_kb = detect_memory();
    uint32_t total_blocks = (total_memory_kb * 1024) / MEMORY_BLOCK_SIZE;

    if (total_blocks > MEMORY_TOTAL_BLOCKS) {
        total_blocks = MEMORY_TOTAL_BLOCKS;
    }

    mem_info.total_blocks = total_blocks;

    // Initialize bitmap to all free
    for (uint32_t i = 0; i < MEMORY_BITMAP_SIZE; i++) {
        mem_info.bitmap[i] = 0;
    }

    mem_info.used_blocks = 0;
    mem_info.free_blocks = total_blocks;

    // Reserve first 256 blocks for kernel (1MB)
    uint32_t kernel_blocks = 256;
    if (kernel_blocks > total_blocks) {
        kernel_blocks = total_blocks / 2;
    }

    for (uint32_t i = 0; i < kernel_blocks; i++) {
        uint32_t byte = i / 8;
        uint32_t bit = i % 8;
        mem_info.bitmap[byte] |= (1 << bit);
        mem_info.used_blocks++;
        mem_info.free_blocks--;
    }
}

memory_info_t* memory_get_info(void) {
    return &mem_info;
}

uint32_t memory_get_total_kb(void) {
    return (mem_info.total_blocks * MEMORY_BLOCK_SIZE) / 1024;
}

uint32_t memory_get_used_kb(void) {
    return (mem_info.used_blocks * MEMORY_BLOCK_SIZE) / 1024;
}

uint32_t memory_get_free_kb(void) {
    return (mem_info.free_blocks * MEMORY_BLOCK_SIZE) / 1024;
}

void* memory_alloc_block(void) {
    for (uint32_t i = 0; i < mem_info.total_blocks; i++) {
        uint32_t byte = i / 8;
        uint32_t bit = i % 8;

        if (!(mem_info.bitmap[byte] & (1 << bit))) {
            mem_info.bitmap[byte] |= (1 << bit);
            mem_info.used_blocks++;
            mem_info.free_blocks--;
            return (void*)(i * MEMORY_BLOCK_SIZE);
        }
    }
    return NULL;
}

void memory_free_block(void* addr) {
    uint32_t block = (uint32_t)addr / MEMORY_BLOCK_SIZE;
    if (block >= mem_info.total_blocks) return;

    uint32_t byte = block / 8;
    uint32_t bit = block % 8;

    mem_info.bitmap[byte] &= ~(1 << bit);
    mem_info.used_blocks--;
    mem_info.free_blocks++;
}
