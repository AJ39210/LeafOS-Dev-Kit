#!/bin/bash
# create_rootfs.sh - Create LeafOS root filesystem structure

echo "Creating LeafOS rootfs structure..."

# Clean up old rootfs
rm -rf rootfs
mkdir -p rootfs

# Create directory structure
mkdir -p rootfs/system/bin
mkdir -p rootfs/system/lib/modules
mkdir -p rootfs/system/etc/init.d
mkdir -p rootfs/system/sbin

mkdir -p rootfs/vendor/drivers/gpu
mkdir -p rootfs/vendor/drivers/wifi
mkdir -p rootfs/vendor/drivers/storage
mkdir -p rootfs/vendor/firmware

mkdir -p rootfs/recovery/bin
mkdir -p rootfs/recovery/etc
mkdir -p rootfs/recovery/logs

mkdir -p rootfs/dev
mkdir -p rootfs/proc
mkdir -p rootfs/sys
mkdir -p rootfs/tmp

# Create placeholder files
cat > rootfs/system/etc/passwd << 'EOF'
root:x:0:0:root:/root:/system/bin/sh
EOF

cat > rootfs/system/etc/group << 'EOF'
root:x:0:
EOF

cat > rootfs/recovery/etc/recovery.conf << 'EOF'
# LeafOS Recovery Configuration
VERSION=1.0
LOG_LEVEL=INFO
EOF

cat > rootfs/recovery/logs/recovery.log << 'EOF'
LeafOS Recovery Log
===================
System initialized
EOF

cat > rootfs/vendor/firmware/README.txt << 'EOF'
Firmware blobs directory
Place binary firmware files here
EOF

# Create a simple text-based init (will be replaced with ELF binary later)
cat > rootfs/system/sbin/init << 'EOF'
LeafOS Init System
==================
This is a placeholder init file.
In a real system, this would be a compiled ELF binary.

To create a real init:
1. Write init.c with your initialization code
2. Compile: gcc -m32 -static -o init init.c
3. Replace this file with the compiled binary
EOF

# Create top-level init that calls system init
cat > rootfs/init << 'EOF'
LeafOS Bootstrap Init
=====================
This init loads the main system init at system/sbin/init
EOF

# Create README
cat > rootfs/README.txt << 'EOF'
LeafOS Root Filesystem
======================

Structure:
- system/     : Core system files (binaries, libraries, configs)
- vendor/     : Vendor-specific drivers and firmware
- recovery/   : Recovery mode files
- dev/        : Device nodes (populated at runtime)
- proc/       : Process filesystem mountpoint
- sys/        : System filesystem mountpoint  
- tmp/        : Temporary files

To add executable binaries:
1. Compile your program as 32-bit static ELF:
   gcc -m32 -static -o myprogram myprogram.c
2. Place in system/bin/ or system/sbin/
3. Rebuild initrd.img with make_initrd.py
EOF

echo "Rootfs structure created in rootfs/"
echo ""
echo "Directory tree:"
tree rootfs/ 2>/dev/null || find rootfs/ -print | sed -e 's;[^/]*/;|____;g;s;____|; |;g'

echo ""
echo "Next steps:"
echo "1. Build init binaries: chmod +x build_init.sh && ./build_init.sh"
echo "2. Create InitRD: ./make_initrd.py initrd.img rootfs"
echo "3. Build and run: make iso && make run"