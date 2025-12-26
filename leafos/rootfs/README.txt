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
