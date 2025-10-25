#include "commands.h"
#include "vga.h"
#include "memory.h"
#include "initrd.h"

// === Minimal string functions for freestanding kernel ===

static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

static int strncmp(const char* s1, const char* s2, int n) {
    while (n-- && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return n < 0 ? 0 : *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static char* strrchr(const char* str, int ch) {
    char* last = 0;
    while (*str) {
        if (*str == (char)ch) last = (char*)str;
        str++;
    }
    return last;
}

static char* strchr(const char* str, int ch) {
    while (*str) {
        if (*str == (char)ch) return (char*)str;
        str++;
    }
    return 0;
}

// === Filesystem globals ===

extern fs_entry_t fs_table[];
extern int fs_count;
extern char current_path[];

// === Filesystem commands ===

void command_ls(void) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write("Listing: ");
    vga_write(current_path);
    vga_write("\n");

    const char* path = current_path;
    if (strcmp(path, "/") == 0) path = "";

    int found = 0;
    for (int i = 0; i < fs_count; i++) {
        if (strncmp(fs_table[i].name, path, strlen(path)) == 0) {
            const char* sub = fs_table[i].name + strlen(path);
            int is_nested = 0;
            for (int j = 0; sub[j]; j++) {
                if (sub[j] == '/') {
                    is_nested = 1;
                    break;
                }
            }
            if (!is_nested) {
                vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                vga_write("  ");
                vga_write(sub);
                if (fs_table[i].type == 1) {
                    vga_write(" [folder]");
                }
                vga_write("\n");
                found = 1;
            }
        }
    }

    if (!found) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_write("No entries found.\n");
    }
}

void command_cd(const char* folder) {
    if (strcmp(folder, "..") == 0) {
        char* last = strrchr(current_path, '/');
        if (last && last != current_path) {
            *last = '\0';
            last = strrchr(current_path, '/');
            if (last) *(last + 1) = '\0';
        } else {
            current_path[0] = '/';
            current_path[1] = '\0';
        }
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_write("Moved up to: ");
        vga_write(current_path);
        vga_write("\n");
        return;
    }

    char temp[64];
    int i = 0;
    for (int j = 0; current_path[j] && i < sizeof(temp) - 1; j++) {
        temp[i++] = current_path[j];
    }
    for (int j = 0; folder[j] && i < sizeof(temp) - 1; j++) {
        temp[i++] = folder[j];
    }
    if (i < sizeof(temp) - 1) temp[i++] = '/';
    temp[i] = '\0';

    for (int k = 0; k < fs_count; k++) {
        if (fs_table[k].type == 1 && strcmp(fs_table[k].name, temp) == 0) {
            int j = 0;
            while (temp[j]) {
                current_path[j] = temp[j];
                j++;
            }
            current_path[j] = '\0';

            vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            vga_write("Changed directory to: ");
            vga_write(current_path);
            vga_write("\n");
            return;
        }
    }

    vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    vga_write("Folder not found.\n");
}

// === Built-in commands ===

void command_help(void) {
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("Available commands:\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("  help      - Show this help message\n");
    vga_write("  clear     - Clear the screen\n");
    vga_write("  bitmap    - Show memory bitmap information\n");
    vga_write("  about     - About this OS\n");
    vga_write("  run X     - Run a program (e.g. run initrd)\n");
    vga_write("  ls        - List files and folders\n");
    vga_write("  cd <name> - Change directory\n");
}

void command_clear(void) {
    vga_clear();
}

void command_bitmap(void) {
    memory_info_t* mem = memory_get_info();

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write("=== Memory Information ===\n");

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Total Memory:  ");
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga_write_dec(memory_get_total_kb());
    vga_write(" KB (");
    vga_write_dec(mem->total_blocks);
    vga_write(" blocks)\n");

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Used Memory:   ");
    vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    vga_write_dec(memory_get_used_kb());
    vga_write(" KB (");
    vga_write_dec(mem->used_blocks);
    vga_write(" blocks)\n");

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Free Memory:   ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_dec(memory_get_free_kb());
    vga_write(" KB (");
    vga_write_dec(mem->free_blocks);
    vga_write(" blocks)\n");

    uint32_t used_percent = (mem->used_blocks * 100) / mem->total_blocks;
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("Usage:         ");
    vga_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga_write_dec(used_percent);
    vga_write("%\n");
}

void command_about(void) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write("LeafOS v9.5\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("A simple operating system built from scratch!\n");
    vga_write("Features:\n");
    vga_write("  - Custom bootloader\n");
    vga_write("  - VGA text mode driver\n");
    vga_write("  - PS/2 keyboard driver\n");
    vga_write("  - Memory management with bitmap\n");
    vga_write("  - Simple shell interface\n");
    vga_write("  - Initrd filesystem with folders\n");
    vga_write("\n");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write("Made with <3 and lots of coffee!\n");
}

void command_run(const char* filename) {
    if (!filename || *filename == '\0') {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_write("Error: No filename provided.\n");
        return;
    }

    if (strcmp(filename, "initrd") == 0) {
        initrd_init();
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_write("Unknown program: ");
        vga_write(filename);
        vga_write("\n");
    }
}
