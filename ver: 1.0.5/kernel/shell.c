// kernel/shell.c
#include "../include/shell.h"
#include "../include/vga.h"
#include "../include/keyboard.h"
#include "../include/cmds.h"

static char input_buffer[SHELL_BUFFER_SIZE];
static int buffer_pos = 0;

// Simple string comparison
static int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

// Simple string length
static int strlen(const char* s) {
    int len = 0;
    while (*s++) len++;
    return len;
}

// Find start of arguments (after command)
static const char* get_args(const char* input) {
    (void)strlen; // May be used later
    while (*input && *input != ' ') input++;
    while (*input == ' ') input++;
    return input;
}

void shell_init(void) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("\n===========================================\n");
    vga_print("         Welcome to LeafOS Shell!\n");
    vga_print("===========================================\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("Type 'help' for available commands.\n\n");
    
    buffer_pos = 0;
    for (int i = 0; i < SHELL_BUFFER_SIZE; i++) {
        input_buffer[i] = 0;
    }
}

void shell_print_prompt(void) {
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print("shell");
    vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    vga_print(">");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print(" ");
}

void shell_process_input(char c) {
    if (c == '\n') {
        vga_putchar('\n');
        if (buffer_pos > 0) {
            input_buffer[buffer_pos] = '\0';
            shell_execute_command();
        }
        buffer_pos = 0;
        for (int i = 0; i < SHELL_BUFFER_SIZE; i++) {
            input_buffer[i] = 0;
        }
        shell_print_prompt();
    } else if (c == '\b') {
        if (buffer_pos > 0) {
            buffer_pos--;
            input_buffer[buffer_pos] = 0;
            vga_putchar('\b');
            vga_putchar(' ');
            vga_putchar('\b');
        }
    } else if (buffer_pos < SHELL_BUFFER_SIZE - 1) {
        input_buffer[buffer_pos++] = c;
        vga_putchar(c);
    }
}

void shell_execute_command(void) {
    // Extract command (before first space)
    const char* args = get_args(input_buffer);
    
    // Compare commands
    if (strcmp(input_buffer, "help") == 0) {
        cmd_help(args);
    } else if (strcmp(input_buffer, "about") == 0) {
        cmd_about(args);
    } else if (input_buffer[0] == 'e' && input_buffer[1] == 'c' && 
               input_buffer[2] == 'h' && input_buffer[3] == 'o' &&
               (input_buffer[4] == ' ' || input_buffer[4] == '\0')) {
        cmd_echo(args);
    } else if (strcmp(input_buffer, "clear") == 0) {
        cmd_clear(args);
    } else if (strcmp(input_buffer, "ls") == 0) {
        cmd_ls(args);
    } else if (input_buffer[0] == 'c' && input_buffer[1] == 'a' && 
               input_buffer[2] == 't' &&
               (input_buffer[3] == ' ' || input_buffer[3] == '\0')) {
        cmd_cat(args);
    } else if (input_buffer[0] == 't' && input_buffer[1] == 'o' && 
               input_buffer[2] == 'u' && input_buffer[3] == 'c' &&
               input_buffer[4] == 'h' &&
               (input_buffer[5] == ' ' || input_buffer[5] == '\0')) {
        cmd_touch(args);
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print("Unknown command: ");
        vga_print(input_buffer);
        vga_print("\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga_print("Type 'help' for available commands.\n");
    }
}

void shell_run(void) {
    shell_print_prompt();
    
    while (1) {
        char c = keyboard_getchar();
        if (c != 0) {
            shell_process_input(c);
            // Wait for key release to prevent repeats
            while (keyboard_getchar() != 0);
        }
        for (volatile int i = 0; i < 5000; i++);
    }
}