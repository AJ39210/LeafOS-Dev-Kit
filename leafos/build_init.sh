#!/bin/bash
# build_init.sh - Build init binaries for LeafOS

echo "Building LeafOS init binaries..."

# Create userspace directory for source files
mkdir -p userspace

# Create syscall implementation (stub that will be resolved by kernel)
cat > userspace/syscall.c << 'EOF'
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
EOF

# Create syscall header
cat > userspace/syscall.h << 'EOF'
// syscall.h - Syscall interface for userspace programs
#ifndef SYSCALL_H
#define SYSCALL_H

void sys_write(const char* str);
void sys_exit(int code);

#endif
EOF

# Create the main init program (system/sbin/init)
cat > userspace/init.c << 'EOF'
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
EOF

# Create the bootstrap init (top-level init)
cat > userspace/bootstrap_init.c << 'EOF'
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
EOF

# Create a test hello program
cat > userspace/hello.c << 'EOF'
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
EOF

# Compile syscall stubs
echo "Compiling syscall stubs..."
gcc -m32 -c -nostdlib -fno-builtin -ffreestanding -fno-stack-protector -o userspace/syscall.o userspace/syscall.c

# Link with custom linker script for flat binary
cat > userspace/link.ld << 'EOF'
OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)

SECTIONS
{
    . = 0x0;
    
    .text : {
        *(.text)
        *(.text.*)
    }
    
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    
    .data : {
        *(.data)
        *(.data.*)
    }
    
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
}
EOF

# Compile the binaries
echo "Compiling init..."
gcc -m32 -c -nostdlib -fno-builtin -ffreestanding -fno-stack-protector -I userspace -o userspace/init.o userspace/init.c
ld -m elf_i386 -T userspace/link.ld -o userspace/init userspace/init.o userspace/syscall.o

echo "Compiling bootstrap_init..."
gcc -m32 -c -nostdlib -fno-builtin -ffreestanding -fno-stack-protector -I userspace -o userspace/bootstrap_init.o userspace/bootstrap_init.c
ld -m elf_i386 -T userspace/link.ld -o userspace/bootstrap_init userspace/bootstrap_init.o userspace/syscall.o

echo "Compiling hello..."
gcc -m32 -c -nostdlib -fno-builtin -ffreestanding -fno-stack-protector -I userspace -o userspace/hello.o userspace/hello.c
ld -m elf_i386 -T userspace/link.ld -o userspace/hello userspace/hello.o userspace/syscall.o

# Verify binaries were created
if [ ! -f userspace/init ]; then
    echo "ERROR: Failed to create init binary"
    exit 1
fi

# Copy to rootfs
echo "Copying binaries to rootfs..."
mkdir -p rootfs/system/sbin
mkdir -p rootfs/system/bin

cp userspace/init rootfs/system/sbin/init
cp userspace/bootstrap_init rootfs/init
cp userspace/hello rootfs/system/bin/hello

echo ""
echo "Build complete!"
echo "Binaries created:"
echo "  - rootfs/system/sbin/init (main init)"
echo "  - rootfs/init (bootstrap)"
echo "  - rootfs/system/bin/hello (test program)"
echo ""
echo "File info:"
ls -lh userspace/init userspace/bootstrap_init userspace/hello
echo ""
echo "Verifying ELF files..."
file userspace/init userspace/bootstrap_init userspace/hello
echo ""
echo "Now run: ./make_initrd.py initrd.img rootfs"
echo "Then: make iso && make run"