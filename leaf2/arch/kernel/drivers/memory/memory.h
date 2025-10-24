#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"

#define MEMORY_BLOCK_SIZE 4096      // 4KB blocks
#define MEMORY_TOTAL_BLOCKS 16384   // Maximum 16384 blocks (64MB)
#define MEMORY_BITMAP_SIZE 2048     // 16384 bits / 8 = 2048 bytes

// Memory bitmap structure
typedef struct {
    uint8_t* bitmap;           // dynamically sized bitmap
    uint32_t total_blocks;     // total number of blocks
    uint32_t used_blocks;      // blocks marked as used
    uint32_t free_blocks;      // blocks marked as free
} memory_info_t;

// Initialize memory management
void memory_init(void);

// Get memory info
memory_info_t* memory_get_info(void);

// Allocate a block
void* memory_alloc_block(void);

// Free a block
void memory_free_block(void* addr);

// Get total memory in KB
uint32_t memory_get_total_kb(void);

// Get used memory in KB
uint32_t memory_get_used_kb(void);

// Get free memory in KB
uint32_t memory_get_free_kb(void);

#endif