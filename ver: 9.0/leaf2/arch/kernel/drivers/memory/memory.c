#include "memory.h"

static memory_info_t mem_info;

void memory_init(void) {
    // Initialize bitmap to all free (0 = free, 1 = used)
    for (uint32_t i = 0; i < MEMORY_BITMAP_SIZE; i++) {
        mem_info.bitmap[i] = 0;
    }
    
    mem_info.total_blocks = MEMORY_TOTAL_BLOCKS;
    mem_info.used_blocks = 0;
    mem_info.free_blocks = MEMORY_TOTAL_BLOCKS;
    
    // Mark first 256 blocks as used (kernel space: 0x00000000 - 0x00100000)
    for (uint32_t i = 0; i < 256; i++) {
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
    // Find first free block
    for (uint32_t i = 0; i < mem_info.total_blocks; i++) {
        uint32_t byte = i / 8;
        uint32_t bit = i % 8;
        
        if (!(mem_info.bitmap[byte] & (1 << bit))) {
            // Mark as used
            mem_info.bitmap[byte] |= (1 << bit);
            mem_info.used_blocks++;
            mem_info.free_blocks--;
            return (void*)(i * MEMORY_BLOCK_SIZE);
        }
    }
    return NULL;  // Out of memory
}

void memory_free_block(void* addr) {
    uint32_t block = (uint32_t)addr / MEMORY_BLOCK_SIZE;
    if (block >= mem_info.total_blocks) return;
    
    uint32_t byte = block / 8;
    uint32_t bit = block % 8;
    
    // Mark as free
    mem_info.bitmap[byte] &= ~(1 << bit);
    mem_info.used_blocks--;
    mem_info.free_blocks++;
}