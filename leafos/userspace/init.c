// init.c - LeafOS Init System

#include "syscall.h"

void _start() {
    sys_write("=======================================\n");
    sys_write("    LeafOS Init System Starting...    \n");
    sys_write("=======================================\n");
    sys_write("\n");
    sys_write("[INIT] Initializing system services...\n");
    sys_write("[INIT] Mounting filesystems...\n");
    sys_write("[INIT] Starting device manager...\n");
    sys_write("[INIT] Loading drivers...\n");
    sys_write("\n");
    sys_write("HIIIIIIIII from system/sbin/init!\n");
    sys_write("\n");
    sys_write("[INIT] System initialization complete!\n");
    sys_write("=======================================\n");
    sys_write("\n");
    
    sys_exit(0);
}
