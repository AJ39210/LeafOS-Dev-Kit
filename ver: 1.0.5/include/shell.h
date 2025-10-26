// include/shell.h
#ifndef SHELL_H
#define SHELL_H

#define SHELL_BUFFER_SIZE 256

void shell_init(void);
void shell_run(void);
void shell_print_prompt(void);
void shell_process_input(char c);
void shell_execute_command(void);

#endif