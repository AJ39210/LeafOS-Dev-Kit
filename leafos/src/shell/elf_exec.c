// src/shell/elf_exec.c - ELF executable loader and runner

#define DRIVER_VERSION "1.0.0"
#define DRIVER_TYPE "System Driver"
#define DRIVER_NAME "ELF Executable Loader"

#include "elf_exec.h"
#include "../fs/vfs.h"
#include <stdint.h>
#include <stddef.h>

// Forward declarations
void vga_print(const char* data);
void vga_putchar(char c);
void vga_setcolor(uint8_t color);
void vga_print_hex(uint32_t n);

// Syscall interface
extern void sys_write(const char* str);
extern void sys_exit(int code);

// ELF Header structures
typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} elf_header_t;

// Validate ELF file
static int elf_validate(uint8_t* data) {
    elf_header_t* header = (elf_header_t*)data;
    
    // Check ELF magic number
    if (header->e_ident[0] != 0x7F ||
        header->e_ident[1] != 'E' ||
        header->e_ident[2] != 'L' ||
        header->e_ident[3] != 'F') {
        return 0;
    }
    
    // Check if 32-bit
    if (header->e_ident[4] != 1) {
        vga_setcolor(0x0C);
        vga_print("[ERROR] Only 32-bit ELF supported\n");
        vga_setcolor(0x07);
        return 0;
    }
    
    // Check if little-endian
    if (header->e_ident[5] != 1) {
        vga_setcolor(0x0C);
        vga_print("[ERROR] Only little-endian ELF supported\n");
        vga_setcolor(0x07);
        return 0;
    }
    
    return 1;
}

// Execute ELF binary from buffer
int elf_execute_buffer(uint8_t* data, uint32_t size) {
    (void)size;
    
    if (!elf_validate(data)) {
        return -1;
    }
    
    elf_header_t* elf = (elf_header_t*)data;
    
    vga_setcolor(0x0B); // Cyan
    vga_print("[ELF] Loading executable...\n");
    vga_print("[ELF] Entry point: 0x");
    vga_print_hex(elf->e_entry);
    vga_putchar('\n');
    vga_setcolor(0x07);
    
    // Get entry point - for flat binaries loaded in memory, 
    // the entry point is relative to the start of the file
    void (*entry_point)(void) = (void (*)(void))((uint32_t)data + elf->e_entry);
    
    vga_setcolor(0x0A); // Green
    vga_print("[ELF] Executing binary...\n\n");
    vga_setcolor(0x07);
    
    // Call the entry point
    entry_point();
    
    vga_setcolor(0x0A);
    vga_print("\n[ELF] Binary execution completed\n");
    vga_setcolor(0x07);
    
    return 0;
}

// Execute ELF binary from file in InitRD
int elf_execute_file(const char* filename) {
    file_node_t* root = vfs_get_root();
    if (!root) {
        vga_setcolor(0x0C);
        vga_print("[ERROR] No filesystem mounted\n");
        vga_setcolor(0x07);
        return -1;
    }
    
    // Find the file
    file_node_t* file = root->finddir(root, filename);
    if (!file) {
        vga_setcolor(0x0C);
        vga_print("[ERROR] File not found: ");
        vga_print(filename);
        vga_putchar('\n');
        vga_setcolor(0x07);
        return -1;
    }
    
    // Read file contents
    static uint8_t buffer[16384]; // 16KB max, static to avoid stack overflow
    if (file->length > sizeof(buffer)) {
        vga_setcolor(0x0C);
        vga_print("[ERROR] File too large (max 16KB)\n");
        vga_setcolor(0x07);
        return -1;
    }
    
    uint32_t size = file->read(file, 0, file->length, buffer);
    
    vga_setcolor(0x0A);
    vga_print("[EXEC] Loading: ");
    vga_print(filename);
    vga_putchar('\n');
    vga_setcolor(0x07);
    
    return elf_execute_buffer(buffer, size);
}