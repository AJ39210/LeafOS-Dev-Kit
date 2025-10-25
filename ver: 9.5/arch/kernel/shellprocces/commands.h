#ifndef COMMANDS_H
#define COMMANDS_H

// Available shell commands
void command_help(void);
void command_clear(void);
void command_bitmap(void);
void command_about(void);
void command_ls(void);
void command_run(const char* filename);

#endif