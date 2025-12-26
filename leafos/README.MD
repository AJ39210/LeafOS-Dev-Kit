# MyOS - Custom Operating System

A simple operating system built from scratch with GRUB bootloader, VGA/VESA drivers, keyboard support, and InitRD filesystem.

## Features

- **GRUB Bootloader**: Multiboot-compliant boot process
- **VGA Text Driver**: 80x25 text mode display with color support
- **VESA Graphics Driver**: Framework for graphics mode (requires VBE info)
- **PS/2 Keyboard Driver**: Basic keyboard input support
- **Virtual File System (VFS)**: Abstraction layer for file operations
- **InitRD Support**: Initial RAM disk for loading files at boot
- **Structured Codebase**: Well-organized source tree

## Driver Classifications

Each driver includes definitions:
```c
#define DRIVER_VERSION "1.0.0"
#define DRIVER_TYPE "Hardware Driver"  // Types: Hardware, System, Peripheral, Software
#define DRIVER_NAME "Driver Name"
```

### Driver Types
- **System Drivers**: Core OS functionality (Kernel, VFS, InitRD)
- **Hardware Drivers**: Direct hardware interaction (VGA, VESA)
- **Peripheral Drivers**: Input/output devices (Keyboard)
- **Software Drivers**: Virtual or software-based drivers

## Project Structure

```
myos/
├── boot/
│   ├── boot.asm          # Bootloader entry point
│   ├── gdt_flush.asm     # GDT loading
│   └── idt_flush.asm     # IDT loading
├── src/
│   ├── kernel/
│   │   ├── kernel.c      # Main kernel
│   │   └── kernel.h
│   ├── drivers/
│   │   ├── vga.c/h       # VGA text driver
│   │   ├── vesa.c/h      # VESA graphics driver
│   │   └── keyboard.c/h  # Keyboard driver
│   ├── fs/
│   │   ├── vfs.c/h       # Virtual filesystem
│   │   └── initrd.c/h    # Initial RAM disk
│   └── lib/
│       └── string.c/h    # String utilities
├── include/
│   └── types.h           # Type definitions
├── Makefile
├── linker.ld
└── README.md
```

## Prerequisites

### Ubuntu/Debian
```bash
sudo apt install build-essential nasm grub-pc-bin xorriso qemu-system-x86
```

### Cross-Compiler (i686-elf-gcc)
You need a cross-compiler for i686-elf target:

```bash
# Download and build binutils and gcc for i686-elf
# Follow: https://wiki.osdev.org/GCC_Cross-Compiler

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
```

## Building

```bash
# Build kernel
make all

# Create bootable ISO
make iso

# Run in QEMU
make run

# Clean build files
make clean
```

## Creating an InitRD with Proper Filesystem Structure

LeafOS now executes compiled ELF binaries instead of shell scripts! The rootfs follows a standard Unix-like structure.

### Quick Start

```bash
# 1. Create the rootfs structure
chmod +x create_rootfs.sh
./create_rootfs.sh

# 2. (Optional) Add your compiled binaries
# Compile a simple init program:
cat > init.c << 'EOF'
void _start() {
    // Your init code here
    // This would initialize the system
    while(1);
}
EOF
gcc -m32 -static -nostdlib -o rootfs/system/sbin/init init.c

# 3. Generate InitRD
./make_initrd.py initrd.img rootfs

# 4. Build and run
make iso
make run
```

### Rootfs Structure

```
rootfs/
├── system/           # Core system files
│   ├── bin/          # User binaries
│   ├── sbin/         # System binaries (init goes here!)
│   ├── lib/          # Libraries
│   │   └── modules/  # Kernel modules
│   └── etc/          # Configuration files
│       ├── passwd
│       ├── group
│       └── init.d/   # Init scripts
│
├── vendor/           # Vendor-specific files
│   ├── drivers/      # Hardware drivers
│   │   ├── gpu/
│   │   ├── wifi/
│   │   └── storage/
│   └── firmware/     # Firmware blobs
│
├── recovery/         # Recovery mode
│   ├── bin/          # Recovery tools
│   ├── etc/          # Recovery config
│   └── logs/         # Recovery logs
│
├── dev/              # Device nodes (runtime)
├── proc/             # Process filesystem (runtime)
├── sys/              # System filesystem (runtime)
├── tmp/              # Temporary files
└── init              # Bootstrap init (calls system/sbin/init)
```

### Creating Executable Binaries

LeafOS can execute ELF binaries. Here's how to create them:

```bash
# Simple "Hello World" program
cat > hello.c << 'EOF'
// Syscall helpers (these would normally be in libc)
void print(const char* str) {
    // This is a placeholder - real implementation would use syscalls
    // For now, the program just validates as ELF
}

void _start() {
    print("Hello from LeafOS!\n");
    while(1);  // Hang
}
EOF

# Compile as 32-bit static ELF
gcc -m32 -static -nostdlib -o rootfs/system/bin/hello hello.c

# Add to InitRD
./make_initrd.py initrd.img rootfs

# Build and test
make iso && make run
```

### Boot Process

1. **GRUB** loads the kernel and InitRD module
2. **Kernel** initializes VGA, keyboard, VFS
3. **Kernel** mounts InitRD as root filesystem  
4. **Kernel** looks for `system/sbin/init` ELF binary
5. If not found, looks for top-level `init`
6. **ELF loader** validates and prepares the binary
7. (Future) Execute the init binary

### Current Limitations

- ELF execution requires full memory management (paging, heap)
- For now, the ELF loader validates and shows info about binaries
- Full execution will be implemented when memory management is added

### Files in InitRD

All files maintain their directory structure:

```bash
# Updated make_initrd.py with folder support
cat > make_initrd.py << 'EOF'
#!/usr/bin/env python3
import struct
import sys
import os

def create_initrd(paths, output):
    magic = 0x494E5452  # "INTR"
    
    # Collect all files and directories
    entries = []
    file_data_list = []
    
    for path in paths:
        if os.path.isdir(path):
            # Add directory and all its contents
            for root, dirs, files in os.walk(path):
                # Add directory itself
                rel_path = os.path.relpath(root, '.')
                if rel_path != '.':
                    entries.append({
                        'name': rel_path,
                        'is_dir': True,
                        'data': b''
                    })
                
                # Add files in this directory
                for filename in files:
                    file_path = os.path.join(root, filename)
                    rel_path = os.path.relpath(file_path, '.')
                    with open(file_path, 'rb') as f:
                        data = f.read()
                    entries.append({
                        'name': rel_path,
                        'is_dir': False,
                        'data': data
                    })
        else:
            # Single file
            basename = os.path.basename(path)
            with open(path, 'rb') as f:
                data = f.read()
            entries.append({
                'name': basename,
                'is_dir': False,
                'data': data
            })
    
    file_count = len(entries)
    header_size = 8
    entry_size = 128 + 4 + 4 + 4  # name + offset + length + is_directory
    entries_size = entry_size * file_count
    data_offset = header_size + entries_size
    
    with open(output, 'wb') as out:
        # Write header
        out.write(struct.pack('<II', magic, file_count))
        
        # Write file entries
        current_offset = data_offset
        for entry in entries:
            name = entry['name'].encode('ascii').ljust(128, b'\0')
            out.write(name)
            out.write(struct.pack('<I', current_offset if not entry['is_dir'] else 0))
            out.write(struct.pack('<I', len(entry['data'])))
            out.write(struct.pack('<I', 1 if entry['is_dir'] else 0))
            current_offset += len(entry['data'])
        
        # Write file data
        for entry in entries:
            if not entry['is_dir']:
                out.write(entry['data'])
    
    print(f"Created {output} with {file_count} entries:")
    for entry in entries:
        entry_type = "[DIR]" if entry['is_dir'] else "[FILE]"
        print(f"  {entry_type} {entry['name']}")

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: make_initrd.py output.img file1 dir1 file2 ...")
        sys.exit(1)
    
    output = sys.argv[1]
    paths = sys.argv[2:]
    create_initrd(paths, output)
EOF

chmod +x make_initrd.py

# Create folder structure
mkdir -p bin
mkdir -p etc
mkdir -p scripts

# Create files in folders
cat > bin/hello.sh << 'EOF'
#!/bin/sh
echo "Hello from bin/hello.sh!"
EOF

cat > etc/config.txt << 'EOF'
LeafOS Configuration File
Version: 1.0
EOF

cat > scripts/test.sh << 'EOF'
#!/bin/sh
echo "Running test script..."
ls
EOF

# Create root init.sh
cat > init.sh << 'EOF'
#!/bin/sh
echo "==================================="
echo "   Welcome to LeafOS!"
echo "==================================="
echo ""
echo "Files and folders in rootfs:"
ls
echo ""
echo "Reading etc/config.txt:"
cat etc/config.txt
EOF

# Generate InitRD with folders
./make_initrd.py initrd.img init.sh bin etc scripts

# Build and run
make iso
make run
```

## Testing

### QEMU
```bash
qemu-system-i386 -cdrom myos.iso
```

### VirtualBox
1. Create new VM (Type: Other, Version: Other/Unknown)
2. Attach myos.iso as CD-ROM
3. Boot from CD

### Real Hardware (Advanced)
```bash
# Write ISO to USB drive (BE CAREFUL!)
sudo dd if=myos.iso of=/dev/sdX bs=4M status=progress
```

## Keyboard Testing

Once booted, you can type on the keyboard. Characters will appear on screen through the VGA driver.

## Next Steps

1. **Interrupt Handling**: Implement full interrupt descriptor table
2. **Memory Management**: Add paging and heap allocation
3. **Process Management**: Implement multitasking
4. **File Systems**: Add ext2 or FAT32 support
5. **Shell**: Create a basic command-line interface
6. **Userspace**: Implement user mode and system calls

## Known Limitations

- VESA driver requires multiboot2 VBE info (currently placeholder)
- Keyboard interrupts not fully implemented (uses polling)
- No memory management or paging
- InitRD is read-only
- No write support in VFS

## Resources

- [OSDev Wiki](https://wiki.osdev.org/)
- [GRUB Manual](https://www.gnu.org/software/grub/manual/)
- [Intel x86 Manual](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)

## License

This is educational code - feel free to use and modify as needed.