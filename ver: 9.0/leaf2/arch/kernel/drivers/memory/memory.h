#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

#define MEMORY_BLOCK_SIZE 4096  // 4KB blocks
#define MEMORY_TOTAL_BLOCKS 1024  // 4MB total memory
#define MEMORY_BITMAP_SIZE (MEMORY_TOTAL_BLOCKS / 8)

// Memory bitmap structure
typedef struct {
    uint8_t bitmap[MEMORY_BITMAP_SIZE];
    uint32_t total_blocks;
    uint32_t used_blocks;
    uint32_t free_blocks;
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