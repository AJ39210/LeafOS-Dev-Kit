// bootstrap_init.c - Bootstrap init that calls system/sbin/init

#include "syscall.h"

void _start() {
    sys_write("=======================================\n");
    sys_write("   LeafOS Bootstrap Init             \n");
    sys_write("=======================================\n");
    sys_write("\n");
    sys_write("[BOOTSTRAP] Starting...\n");
    sys_write("[BOOTSTRAP] This would load system/sbin/init\n");
    sys_write("[BOOTSTRAP] For now, just a simple test\n");
    sys_write("\n");
    sys_write("HIIIIIIIII from bootstrap init!\n");
    sys_write("\n");
    
    sys_exit(0);
}
