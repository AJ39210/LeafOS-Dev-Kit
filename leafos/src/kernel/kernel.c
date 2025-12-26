// src/kernel/kernel.c - Main kernel initialization and entry point
#define DRIVER_VERSION "2.0.0"
#define DRIVER_TYPE "System Driver"
#define DRIVER_NAME "Kernel Core"
#include <stdint.h>
#include <stddef.h>
#include "../fs/vfs.h"      // use full definitions for file_node_t, dirent_t
// Forward declarations
void vga_init(void);
void vga_clear(void);
void vga_print(const char* data);
void vga_print_hex(uint32_t n);
void vga_putchar(char c);
void vga_setcolor(uint8_t color);
void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
// Keyboard driver
void keyboard_init(void);
void keyboard_poll(void);
// Filesystem
void initrd_init(uint32_t location, uint32_t size);
void vfs_init(void);
file_node_t* vfs_get_root(void);

// Shell
int elf_execute_file(const char* filename);
// Multiboot structure definitions
typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
} multiboot_info_t;
typedef struct {
    uint32_t mod_start;
    uint32_t mod_end;
    uint32_t cmdline;
    uint32_t pad;
} multiboot_module_t;
// Port I/O functions
void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}
uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
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
// ELF Parser
int elf_validate(uint8_t* data) {
    elf_header_t* header = (elf_header_t*)data;
    // Check ELF magic number
    if (header->e_ident[0] != 0x7F ||
        header->e_ident[1] != 'E' ||
        header->e_ident[2] != 'L' ||
        header->e_ident[3] != 'F') {
        return 0;
    }
    return 1;
}
void print_banner(void) {
    vga_setcolor(0x0A); // Light green
    vga_print("  _                  __ ___  ____  \n");
    vga_print(" | |    ___  __ _ / _/ _ \\/ ___| \n");
    vga_print(" | |   / _ \\/ _` | |_ | | \\___ \\ \n");
    vga_print(" | |__|  __/ (_| |  _|| |  ___) |\n");
    vga_print(" |_____\\___|\\__,_|_|  |_| |____/ \n");
    vga_setcolor(0x07); // Back to light grey
    vga_print("\n");
    vga_print("=========================================\n");
    vga_print("          LeafOS v1.0.0 - 2024          \n");
    vga_print("=========================================\n\n");
}
void kernel_main(uint32_t magic, multiboot_info_t* mboot_info) {
    // Initialize VGA driver first for output
    vga_init();
    vga_clear();
    // Print banner
    print_banner();
    // Verify multiboot magic
    vga_print("[BOOT] Checking multiboot header...\n");
    if (magic != 0x2BADB002) {
        vga_setcolor(0x0C); // Light red
        vga_print("[ERROR] Invalid multiboot magic: 0x");
        vga_print_hex(magic);
        vga_print("\n");
        vga_print("[ERROR] Expected: 0x2BADB002\n");
        while(1) { asm("hlt"); }
    }
    vga_setcolor(0x0A); // Light green
    vga_print("[OK] Multiboot header valid: 0x");
    vga_print_hex(magic);
    vga_print("\n");
    vga_setcolor(0x07); // Back to grey
    // Initialize drivers
    vga_print("[DRIVER] Loading VGA Text Mode Driver v1.0.0\n");
    vga_print("[DRIVER] Type: Hardware Driver\n");
    vga_print("[DRIVER] Loading PS/2 Keyboard Driver v1.0.0\n");
    vga_print("[DRIVER] Type: Peripheral Driver\n");
    keyboard_init();
    vga_setcolor(0x0A);
    vga_print("[OK] Keyboard driver loaded\n");
    vga_setcolor(0x07);
    // Initialize VFS
    vga_print("[FS] Initializing Virtual Filesystem...\n");
    vfs_init();
    vga_setcolor(0x0A);
    vga_print("[OK] VFS initialized\n");
    vga_setcolor(0x07);
    // Load initrd if present
    vga_print("[FS] Checking for InitRD...\n");
    if (mboot_info->mods_count > 0) {
        multiboot_module_t* modules = (multiboot_module_t*)mboot_info->mods_addr;
        uint32_t initrd_location = modules[0].mod_start;
        uint32_t initrd_end = modules[0].mod_end;
        uint32_t initrd_size = initrd_end - initrd_location;
        vga_print("[FS] InitRD found at: 0x");
        vga_print_hex(initrd_location);
        vga_print("\n");
        vga_print("[FS] InitRD size: 0x");
        vga_print_hex(initrd_size);
        vga_print(" bytes\n");
        // Check if it's an ELF file
        vga_print("[ELF] Starting ELF parser...\n");
        uint8_t* initrd_data = (uint8_t*)initrd_location;
        if (elf_validate(initrd_data)) {
            vga_setcolor(0x0A);
            vga_print("[OK] Valid ELF format detected\n");
            vga_setcolor(0x07);
            elf_header_t* elf = (elf_header_t*)initrd_data;
            vga_print("[ELF] Entry point: 0x");
            vga_print_hex(elf->e_entry);
            vga_print("\n");
        } else {
            vga_print("[ELF] Not an ELF file, treating as InitRD filesystem\n");
        }
        vga_print("[FS] Starting InitRD reader...\n");
        initrd_init(initrd_location, initrd_size);
        vga_setcolor(0x0A);
        vga_print("[OK] InitRD mounted successfully\n");
        vga_setcolor(0x07);
        // List files in initrd
        vga_print("\n[FS] Files in rootfs:\n");
        file_node_t* root = vfs_get_root();
        if (root) {
            uint32_t i = 0;
            dirent_t* dent;
            while ((dent = root->readdir(root, i)) != NULL) {
                vga_print("  -> ");
                vga_print(dent->name);
                vga_print("\n");
                i++;
            }
            if (i == 0) {
                vga_print("  (empty rootfs)\n");
            }
        }
        
        // Try to execute system/sbin/init if it exists
        vga_print("\n[INIT] Looking for system/sbin/init...\n");
        file_node_t* init_binary = root ? root->finddir(root, "system/sbin/init") : 0;
        if (init_binary) {
            vga_setcolor(0x0A);
            vga_print("[OK] Found init binary, executing...\n\n");
            vga_setcolor(0x07);
            elf_execute_file("system/sbin/init");
            vga_print("\n");
        } else {
            // Try top-level init as fallback
            vga_print("[INIT] Looking for init...\n");
            init_binary = root ? root->finddir(root, "init") : 0;
            if (init_binary) {
                vga_setcolor(0x0A);
                vga_print("[OK] Found init, executing...\n\n");
                vga_setcolor(0x07);
                elf_execute_file("init");
                vga_print("\n");
            } else {
                vga_setcolor(0x0E);
                vga_print("[INFO] No init found, continuing to shell\n");
                vga_setcolor(0x07);
            }
        }
    } else {
        vga_setcolor(0x0E); // Yellow
        vga_print("[WARN] No InitRD module found\n");
        vga_setcolor(0x07);
    }
    vga_print("\n=========================================\n");
    vga_setcolor(0x0A);
    vga_print("         System Ready - Type Away!       \n");
    vga_setcolor(0x07);
    vga_print("=========================================\n\n");
    // Main kernel loop - poll keyboard
    while(1) {
        keyboard_poll();
        // Small delay
        for(volatile int i = 0; i < 10000; i++);
    }
}