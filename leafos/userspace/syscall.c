// syscall.c - Syscall stubs for userspace

// These are weak symbols that will be overridden by kernel implementations
// For now, we'll use inline assembly to define them as dummy functions

__attribute__((section(".text")))
void sys_write(const char* str) {
    // This will be provided by the kernel at runtime
    (void)str;
}

__attribute__((section(".text")))
void sys_exit(int code) {
    // This will be provided by the kernel at runtime
    (void)code;
    while(1); // Hang if kernel doesn't provide this
}
