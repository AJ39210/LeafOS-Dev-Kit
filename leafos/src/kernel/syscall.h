// src/kernel/syscalls.h - Syscall interface

#ifndef SYSCALLS_H
#define SYSCALLS_H

// Syscall functions
void sys_write(const char* str);
void sys_exit(int code);

#endif