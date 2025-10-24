#include "memory.h"

static memory_info_t mem_info;

// Detect total RAM by reading from BIOS data area
// This reads the value set by the bootloader/BIOS
static uint32_t detect_memory(void) {
    // Read extended memory size from BIOS data area (in KB)
    // Located at 0x413 (base memory in KB)
    uint16_t* base_mem_ptr = (uint16_t*)0x413;
    uint32_t base_memory = *base_mem_ptr;
    
    // For simplicity, assume we have base memory + some extended memory
    // In a real system, you'd use INT 0x15, EAX=0xE820 during boot
    // For now, let's assume 16MB total (common for emulators)
    uint32_t total_kb = 16 * 1024;  // 16MB in KB
    
    return total_kb;
}

void memory_init(void) {
    // Detect total memory
    uint32_t total_memory_kb = detect_memory();
    uint32_t total_blocks = (total_memory_kb * 1024) / MEMORY_BLOCK_SIZE;
    
    // Ensure we don't exceed our bitmap size
    if (total_blocks > MEMORY_TOTAL_BLOCKS) {
        total_blocks = MEMORY_TOTAL_BLOCKS;
    }
    
    mem_info.total_blocks = total_blocks;
    
    // Initialize bitmap to all free (0 = free, 1 = used)
    for (uint32_t i = 0; i < MEMORY_BITMAP_SIZE; i++) {
        mem_info.bitmap[i] = 0;
    }
    
    mem_info.used_blocks = 0;
    mem_info.free_blocks = total_blocks;
    
    // Mark first 256 blocks as used (kernel space: 0x00000000 - 0x00100000 = 1MB)
    uint32_t kernel_blocks = 256;  // 1MB for kernel
    if (kernel_blocks > total_blocks) {
        kernel_blocks = total_blocks / 2;  // Use half if memory is low
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