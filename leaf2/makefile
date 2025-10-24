# LeafOS Makefile

# Tools
ASM = nasm
CC = gcc
LD = ld

# Directories
ARCH_DIR = arch
BOOT_DIR = $(ARCH_DIR)/boot
KERNEL_DIR = $(ARCH_DIR)/kernel
DRIVERS_DIR = $(KERNEL_DIR)/drivers
VGA_DIR = $(DRIVERS_DIR)/vga
KEYBOARD_DIR = $(DRIVERS_DIR)/keyboard
MEMORY_DIR = $(DRIVERS_DIR)/memory
IDT_DIR = $(DRIVERS_DIR)/idt
INITRD_DIR = $(DRIVERS_DIR)/initrd
SHELL_DIR = $(KERNEL_DIR)/shellprocces

# Flags
ASMFLAGS = -f elf32
BOOT_ASMFLAGS = -f bin
CFLAGS = -m32 -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -O0 -Wall -Wextra -c \
-I$(VGA_DIR) -I$(KEYBOARD_DIR) -I$(MEMORY_DIR) -I$(IDT_DIR) -I$(INITRD_DIR) -I$(SHELL_DIR)

LDFLAGS = -m elf_i386 -T linker.ld

# Sources
BOOT_SRC = $(BOOT_DIR)/boot.asm
KERNEL_C_SRC = $(KERNEL_DIR)/kernel.c
VGA_C_SRC = $(VGA_DIR)/vga.c
KEYBOARD_C_SRC = $(KEYBOARD_DIR)/keyboard.c
MEMORY_C_SRC = $(MEMORY_DIR)/memory.c
IDT_C_SRC = $(IDT_DIR)/idt.c
IDT_ASM_SRC = $(IDT_DIR)/idt_asm.asm
INITRD_C_SRC = $(INITRD_DIR)/initrd.c
INITRD_ASM_SRC = $(INITRD_DIR)/initrd.asm
FILESYSTEM_ASM_SRC = $(INITRD_DIR)/filesystem.asm
SHELL_C_SRC = $(SHELL_DIR)/shell.c
COMMANDS_C_SRC = $(SHELL_DIR)/commands.c

# Objects
KERNEL_OBJ = $(KERNEL_DIR)/kernel.o
VGA_OBJ = $(VGA_DIR)/vga.o
KEYBOARD_OBJ = $(KEYBOARD_DIR)/keyboard.o
MEMORY_OBJ = $(MEMORY_DIR)/memory.o
IDT_OBJ = $(IDT_DIR)/idt.o
IDT_ASM_OBJ = $(IDT_DIR)/idt_asm.o
INITRD_OBJ = $(INITRD_DIR)/initrd.o
INITRD_ASM_OBJ = $(INITRD_DIR)/initrd_asm.o
FILESYSTEM_ASM_OBJ = $(INITRD_DIR)/filesystem.o
SHELL_OBJ = $(SHELL_DIR)/shell.o
COMMANDS_OBJ = $(SHELL_DIR)/commands.o

ALL_OBJ = $(KERNEL_OBJ) $(VGA_OBJ) $(KEYBOARD_OBJ) $(MEMORY_OBJ) $(IDT_OBJ) $(IDT_ASM_OBJ) \
          $(INITRD_OBJ) $(INITRD_ASM_OBJ) $(FILESYSTEM_ASM_OBJ) $(SHELL_OBJ) $(COMMANDS_OBJ)

# Output
BOOT_BIN = $(BOOT_DIR)/boot.bin
KERNEL_BIN = kernel.bin
OS_IMAGE = myos.img

# Targets
.PHONY: all clr run debug help dirs

all: dirs $(OS_IMAGE)

dirs:
	@mkdir -p $(BOOT_DIR)
	@mkdir -p $(KERNEL_DIR)
	@mkdir -p $(VGA_DIR)
	@mkdir -p $(KEYBOARD_DIR)
	@mkdir -p $(MEMORY_DIR)
	@mkdir -p $(IDT_DIR)
	@mkdir -p $(INITRD_DIR)
	@mkdir -p $(SHELL_DIR)

# Bootloader
$(BOOT_BIN): $(BOOT_SRC)
	$(ASM) $(BOOT_ASMFLAGS) $< -o $@

# Kernel
$(KERNEL_OBJ): $(KERNEL_C_SRC)
	$(CC) $(CFLAGS) $< -o $@

# VGA
$(VGA_OBJ): $(VGA_C_SRC)
	$(CC) $(CFLAGS) $< -o $@

# Keyboard
$(KEYBOARD_OBJ): $(KEYBOARD_C_SRC)
	$(CC) $(CFLAGS) $< -o $@

# Memory
$(MEMORY_OBJ): $(MEMORY_C_SRC)
	$(CC) $(CFLAGS) $< -o $@

# IDT
$(IDT_OBJ): $(IDT_C_SRC)
	$(CC) $(CFLAGS) $< -o $@

$(IDT_ASM_OBJ): $(IDT_ASM_SRC)
	$(ASM) $(ASMFLAGS) $< -o $@

# Initrd driver
$(INITRD_OBJ): $(INITRD_C_SRC)
	$(CC) $(CFLAGS) $< -o $@

# Initrd loader stub
$(INITRD_ASM_OBJ): $(INITRD_ASM_SRC)
	$(ASM) $(ASMFLAGS) $< -o $@

# Filesystem
$(FILESYSTEM_ASM_OBJ): $(FILESYSTEM_ASM_SRC)
	$(ASM) $(ASMFLAGS) $< -o $@

# Shell
$(SHELL_OBJ): $(SHELL_C_SRC)
	$(CC) $(CFLAGS) $< -o $@

$(COMMANDS_OBJ): $(COMMANDS_C_SRC)
	$(CC) $(CFLAGS) $< -o $@

# Link
$(KERNEL_BIN): $(ALL_OBJ)
	$(LD) $(LDFLAGS) $(ALL_OBJ) -o kernel.elf
	objcopy -O binary kernel.elf $(KERNEL_BIN)

# Image
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(OS_IMAGE)
	truncate -s 140K $(OS_IMAGE)
	@echo "Build complete: $(OS_IMAGE)"

# Run
run: $(OS_IMAGE)
	qemu-system-i386 -fda $(OS_IMAGE) 

debug: $(OS_IMAGE)
	qemu-system-i386 -fda $(OS_IMAGE) -s -S

clr:
	rm -f $(ALL_OBJ) $(BOOT_BIN) kernel.elf $(KERNEL_BIN) $(OS_IMAGE)

help:
	@echo "LeafOS Build System"
	@echo "Targets:"
	@echo "  all    - Build the OS image"
	@echo "  run    - Run in QEMU"
	@echo "  debug  - Run in QEMU with GDB"
	@echo "  clr    - Clean build artifacts"
