# Makefile for MyOS

# Compiler and flags
CC = gcc
AS = nasm
LD = gcc

CFLAGS = -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude -fno-pie
ASFLAGS = -f elf32
LDFLAGS = -m32 -T linker.ld -ffreestanding -O2 -nostdlib -lgcc -no-pie

# Directories
BUILD_DIR = build
SRC_DIR = src
BOOT_DIR = boot

# Source files
ASM_SOURCES = $(BOOT_DIR)/boot.asm $(BOOT_DIR)/gdt_flush.asm $(BOOT_DIR)/idt_flush.asm
C_SOURCES = $(shell find $(SRC_DIR) -name '*.c')

# Object files
ASM_OBJECTS = $(patsubst $(BOOT_DIR)/%.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

# boot.o MUST be first for multiboot header
OBJECTS = $(BUILD_DIR)/boot.o $(filter-out $(BUILD_DIR)/boot.o,$(ASM_OBJECTS)) $(C_OBJECTS)

# Output
KERNEL = $(BUILD_DIR)/myos.bin
ISO = myos.iso

.PHONY: all clean iso run clr

all: $(KERNEL)

$(KERNEL): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(BUILD_DIR)/%.o: $(BOOT_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

iso: $(KERNEL)
	@mkdir -p isodir/boot/grub
	@cp $(KERNEL) isodir/boot/myos.bin
	@if [ -f initrd.img ]; then \
		cp initrd.img isodir/boot/initrd.img; \
		echo 'menuentry "LeafOS" {' > isodir/boot/grub/grub.cfg; \
		echo '    multiboot /boot/myos.bin' >> isodir/boot/grub/grub.cfg; \
		echo '    module /boot/initrd.img' >> isodir/boot/grub/grub.cfg; \
		echo '}' >> isodir/boot/grub/grub.cfg; \
		echo "ISO created with InitRD"; \
	else \
		echo 'menuentry "LeafOS" {' > isodir/boot/grub/grub.cfg; \
		echo '    multiboot /boot/myos.bin' >> isodir/boot/grub/grub.cfg; \
		echo '}' >> isodir/boot/grub/grub.cfg; \
		echo "Warning: initrd.img not found, ISO created without InitRD"; \
	fi
	@grub-mkrescue -o $(ISO) isodir

run: iso
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -rf $(BUILD_DIR) isodir $(ISO)

clr: clean