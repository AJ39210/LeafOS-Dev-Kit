#include "initrd.h"
#include "vga.h"

fs_entry_t fs_table[MAX_FS_ENTRIES];
int fs_count = 0;
char current_path[64] = "/";

// === Minimal string functions ===

static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static char* strcpy(char* dest, const char* src) {
    char* orig = dest;
    while ((*dest++ = *src++));
    return orig;
}

// === Initrd loader ===

void initrd_init(void) {
    vga_write("Initrd: loading filesystem...\n");

    // Root files
    strcpy(fs_table[fs_count].name, "kernel.bin");
    fs_table[fs_count].type = 0;
    fs_table[fs_count].size = 1024;
    fs_table[fs_count++].data = "Kernel image";

    strcpy(fs_table[fs_count].name, "bootloader.bin");
    fs_table[fs_count].type = 0;
    fs_table[fs_count].size = 512;
    fs_table[fs_count++].data = "Bootloader image";

    // Folders
    strcpy(fs_table[fs_count].name, "bin/");
    fs_table[fs_count++].type = 1;

    strcpy(fs_table[fs_count].name, "home/");
    fs_table[fs_count++].type = 1;

    strcpy(fs_table[fs_count].name, "lib/");
    fs_table[fs_count++].type = 1;

    strcpy(fs_table[fs_count].name, "userspace/");
    fs_table[fs_count++].type = 1;

    // Files inside folders
    strcpy(fs_table[fs_count].name, "bin/echo");
    fs_table[fs_count].type = 0;
    fs_table[fs_count].size = 12;
    fs_table[fs_count++].data = "echo binary";

    strcpy(fs_table[fs_count].name, "lib/etc.o");
    fs_table[fs_count].type = 0;
    fs_table[fs_count].size = 256;
    fs_table[fs_count++].data = "object file";

    strcpy(fs_table[fs_count].name, "lib/etc.bin");
    fs_table[fs_count].type = 0;
    fs_table[fs_count].size = 512;
    fs_table[fs_count++].data = "binary file";

    strcpy(fs_table[fs_count].name, "home/user.txt");
    fs_table[fs_count].type = 0;
    fs_table[fs_count].size = 64;
    fs_table[fs_count++].data = "Welcome to LeafOS";

    vga_write("Initrd: filesystem loaded.\n");
}
