// syscall.h - Syscall interface for userspace programs
#ifndef SYSCALL_H
#define SYSCALL_H

void sys_write(const char* str);
void sys_exit(int code);

#endif
