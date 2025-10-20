# LeafOS Makefile
ASM     = nasm
CC      = gcc
LD      = ld
OBJCOPY = objcopy

ASFLAGS = -f bin
CFLAGS  = -m32 -ffreestanding -O2 -Wall -Wextra -fno-stack-protector -fno-pic -Iarch/kernel/driver
LDFLAGS = -T linker.ld -m elf_i386
OBJS    = build/main.o build/vga.o

.PHONY: all clean run

all: LeafOS.img

# --- Bootsector ---
build/bootsector.bin: arch/boot/bootsector.asm
	@mkdir -p build
	$(ASM) $(ASFLAGS) arch/boot/bootsector.asm -o build/bootsector.bin

# --- Kernel objects ---
build/vga.o: arch/kernel/driver/vga.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c arch/kernel/driver/vga.c -o build/vga.o

build/main.o: arch/kernel/main.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c arch/kernel/main.c -o build/main.o

# --- Link kernel ELF ---
build/kernel.elf: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o build/kernel.elf $(OBJS)

# --- Convert ELF → flat binary ---
build/kernel.bin: build/kernel.elf
	$(OBJCOPY) -O binary build/kernel.elf build/kernel.bin

# --- Build disk image ---
LeafOS.img: build/bootsector.bin build/kernel.bin
	@echo "Creating LeafOS.img..."
	dd if=/dev/zero of=LeafOS.img bs=512 count=2048 status=none
	dd if=build/bootsector.bin of=LeafOS.img conv=notrunc status=none
	dd if=build/kernel.bin of=LeafOS.img bs=512 seek=1 conv=notrunc status=none
	@echo "✅ LeafOS.img built!"

# --- Clean ---
clr:
	rm -rf build *.img

# --- Run in QEMU ---
run: LeafOS.img
	qemu-system-i386 -drive format=raw,file=LeafOS.img
