// hello.c - Simple hello world program

#include "syscall.h"

void _start() {
    sys_write("=======================================\n");
    sys_write("      Hello from LeafOS!              \n");
    sys_write("=======================================\n");
    sys_write("\n");
    sys_write("HIIIIIIIII from hello program!\n");
    sys_write("\n");
    sys_write("This is a compiled C program running\n");
    sys_write("as an ELF binary in LeafOS!\n");
    sys_write("\n");
    
    sys_exit(0);
}
