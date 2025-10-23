#include "shell.h"
#include "vga.h"
#include "keyboard.h"
#include "commands.h"

static char input_buffer[SHELL_BUFFER_SIZE];
static uint32_t buffer_index = 0;

// String length
static uint32_t strlen(const char* str) {
    uint32_t len = 0;
    while (str[len]) len++;
    return len;
}

// String compare
static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void shell_init(void) {
    buffer_index = 0;
    for (uint32_t i = 0; i < SHELL_BUFFER_SIZE; i++) {
        input_buffer[i] = 0;
    }
}

void shell_print_prompt(void) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write("myos");
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
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_write("Unknown command: ");
        vga_write(cmd);
        vga_write("\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga_write("Type 'help' for available commands.\n");
    }
}

// Make this function never return by using noreturn attribute
void shell_run(void) __attribute__((noreturn));

void shell_run(void) {
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    shell_print_prompt();
    
    // Assembly infinite loop that calls our logic
    __asm__ __volatile__(
        "shell_loop:\n"
        "call shell_main_loop\n"
        "jmp shell_loop\n"
    );
    
    __builtin_unreachable();
}

// Separate function for main logic
void shell_main_loop(void) {
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
            vga_putchar('\b');
        }
    } else if (buffer_index < SHELL_BUFFER_SIZE - 1 && c != 0) {
        input_buffer[buffer_index++] = c;
        vga_putchar(c);
    }
}