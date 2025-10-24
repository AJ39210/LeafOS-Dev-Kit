#ifndef SHELL_H
#define SHELL_H

#define SHELL_BUFFER_SIZE 256

// Initialize and run the shell
void shell_init(void);
void shell_run(void);

// Print shell prompt
void shell_print_prompt(void);

// Process command
void shell_process_command(const char* cmd);

#endif