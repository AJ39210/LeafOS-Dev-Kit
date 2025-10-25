#include "shell.h"
#include "vga.h"
#include "keyboard.h"
#include "commands.h"

static char input_buffer[SHELL_BUFFER_SIZE];
static uint32_t buffer_index = 0;

// === Minimal string functions ===

static uint32_t strlen(const char* str) {
    uint32_t len = 0;
    while (str[len]) len++;
    return len;
}

static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static int strncmp(const char* s1, const char* s2, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) return 1;
        if (s1[i] == '\0') break;
    }
    return 0;
}

// === Shell core ===

void shell_init(void) {
    buffer_index = 0;
    for (uint32_t i = 0; i < SHELL_BUFFER_SIZE; i++) {
        input_buffer[i] = 0;
    }
}

void shell_print_prompt(void) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write("Root");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write("> ");
}

void shell_process_command(const char* cmd) {
    if (strlen(cmd) == 0) {
        return;
    }

    if (strcmp(cmd, "help") == 0) {
        command_help();
    } else if (strcmp(cmd, "clear") == 0) {
        command_clear();
    } else if (strcmp(cmd, "bitmap") == 0) {
        command_bitmap();
    } else if (strcmp(cmd, "about") == 0) {
        command_about();
    } else if (strcmp(cmd, "ls") == 0) {
        command_ls();
    } else if (strncmp(cmd, "cd ", 3) == 0) {
        command_cd(cmd + 3);
    } else if (strncmp(cmd, "run ", 4) == 0) {
        command_run(cmd + 4);
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_write("Unknown command: ");
        vga_write(cmd);
        vga_write("\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga_write("Type 'help' for available commands.\n");
    }
}

void shell_run(void) {
    volatile int running = 1;

    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    shell_print_prompt();

    while (running) {
        char c = keyboard_getchar();

        if (c == '\n') {
            vga_putchar('\n');
            input_buffer[buffer_index] = '\0';
            shell_process_command(input_buffer);
            buffer_index = 0;
            shell_print_prompt();
        } else if (c == '\b') {
            if (buffer_index > 0) {
                buffer_index--;
                vga_write("                         \b");  // ← visually erase character
            }
        } else if (buffer_index < SHELL_BUFFER_SIZE - 1 && c != 0) {
            input_buffer[buffer_index++] = c;
            vga_putchar(c);
        }
    }
}
