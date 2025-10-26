# Makefile
CC = gcc
LD = ld
ASM = nasm
PYTHON = python3

CFLAGS = -m32 -c -ffreestanding -O2 -Wall -Wextra -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib
ASMFLAGS = -f elf32

# Directories
BUILD_DIR = build
ISO_DIR = $(BUILD_DIR)/isofiles
BOOT_DIR = $(ISO_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub

# Source files
BOOT_ASM = boot/boot.asm
KERNEL_SRC = kernel/kernel.c
KERNEL_DEBUG_SRC = kernel/kernel_debug.c
VGA_SRC = kernel/vga.c
KEYBOARD_SRC = kernel/keyboard.c
SHELL_SRC = kernel/shell.c
CMDS_SRC = kernel/cmds.c
PANIC_SRC = kernel/panic.c

# Object files
BOOT_OBJ = $(BUILD_DIR)/boot.o
VGA_OBJ = $(BUILD_DIR)/vga.o
KEYBOARD_OBJ = $(BUILD_DIR)/keyboard.o
SHELL_OBJ = $(BUILD_DIR)/shell.o
CMDS_OBJ = $(BUILD_DIR)/cmds.o
PANIC_OBJ = $(BUILD_DIR)/panic.o
KERNEL_OBJ = $(BUILD_DIR)/kernel.o
KERNEL_DEBUG_OBJ = $(BUILD_DIR)/kernel_debug.o

# Common objects
COMMON_OBJS = $(BOOT_OBJ) $(VGA_OBJ) $(KEYBOARD_OBJ) $(SHELL_OBJ) $(CMDS_OBJ) $(PANIC_OBJ)

# Output
ISO = custom_os.iso
KERNEL_BIN = $(BOOT_DIR)/kernel.bin
KERNEL_DEBUG_BIN = $(BOOT_DIR)/kernel_debug.bin

.PHONY: all clean run run-debug dirs background

all: dirs background $(ISO)

dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BOOT_DIR)
	@mkdir -p $(GRUB_DIR)
	@echo "✓ Created directory structure"

background: dirs
	@echo "Generating GRUB background..."
	@$(PYTHON) boot/grub/create_background.py $(GRUB_DIR)/background.png
	@cp boot/grub/grub.cfg $(GRUB_DIR)/grub.cfg
	@echo "✓ GRUB configuration ready"

# Assembly
$(BOOT_OBJ): $(BOOT_ASM)
	@echo "Assembling $<..."
	@$(ASM) $(ASMFLAGS) $< -o $@

# Kernel objects
$(VGA_OBJ): $(VGA_SRC) include/vga.h
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@

$(KEYBOARD_OBJ): $(KEYBOARD_SRC) include/keyboard.h include/vga.h
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@

$(SHELL_OBJ): $(SHELL_SRC) include/shell.h include/vga.h include/keyboard.h include/cmds.h
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@

$(CMDS_OBJ): $(CMDS_SRC) include/cmds.h include/vga.h include/shell.h
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@

$(PANIC_OBJ): $(PANIC_SRC) include/panic.h include/vga.h
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@

$(KERNEL_OBJ): $(KERNEL_SRC) include/vga.h include/keyboard.h include/shell.h include/panic.h
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DEBUG_OBJ): $(KERNEL_DEBUG_SRC) include/vga.h include/keyboard.h
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $< -o $@

# Link kernels
$(KERNEL_BIN): $(COMMON_OBJS) $(KERNEL_OBJ)
	@echo "Linking normal kernel..."
	@$(LD) $(LDFLAGS) $^ -o $@
	@echo "✓ Normal kernel: $@"

$(KERNEL_DEBUG_BIN): $(COMMON_OBJS) $(KERNEL_DEBUG_OBJ)
	@echo "Linking debug kernel..."
	@$(LD) $(LDFLAGS) $^ -o $@
	@echo "✓ Debug kernel: $@"

# Create ISO
$(ISO): $(KERNEL_BIN) $(KERNEL_DEBUG_BIN)
	@echo "Creating bootable ISO..."
	@grub-mkrescue -o $(ISO) $(ISO_DIR) 2>/dev/null
	@echo ""
	@echo "================================================"
	@echo "✓ Build complete!"
	@echo "================================================"
	@echo "ISO file: $(ISO)"
	@echo ""
	@echo "LeafOS has been built successfully!"
	@echo ""
	@echo "To run:"
	@echo "  make run         - Boot in QEMU"
	@echo "  make run-debug   - Boot with debugger"
	@echo ""

run: $(ISO)
	@echo "Booting in QEMU..."
	@qemu-system-i386 -cdrom $(ISO)

run-debug: $(ISO)
	@echo "Starting QEMU with GDB server on :1234"
	@qemu-system-i386 -cdrom $(ISO) -s -S

clr:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR) $(ISO)
	@echo "✓ Clean complete"