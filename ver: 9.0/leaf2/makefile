# Makefile for MyOS

# Compiler and tools
ASM = nasm
CC = gcc
LD = ld

# Directory structure
ARCH_DIR = arch
BOOT_DIR = $(ARCH_DIR)/boot
KERNEL_DIR = $(ARCH_DIR)/kernel
DRIVERS_DIR = $(KERNEL_DIR)/drivers
VGA_DIR = $(DRIVERS_DIR)/vga
KEYBOARD_DIR = $(DRIVERS_DIR)/keyboard
MEMORY_DIR = $(DRIVERS_DIR)/memory
IDT_DIR = $(DRIVERS_DIR)/idt
SHELL_DIR = $(KERNEL_DIR)/shellprocces

# Flags
ASMFLAGS = -f bin
CFLAGS = -m32 -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -O0 -Wall -Wextra -c \
-I$(VGA_DIR) -I$(KEYBOARD_DIR) -I$(MEMORY_DIR) -I$(SHELL_DIR) -I$(IDT_DIR)
LDFLAGS = -m elf_i386 -T linker.ld

# Source files
BOOT_SRC = $(BOOT_DIR)/boot.asm
KERNEL_C_SRC = $(KERNEL_DIR)/kernel.c
VGA_C_SRC = $(VGA_DIR)/vga.c
KEYBOARD_C_SRC = $(KEYBOARD_DIR)/keyboard.c
MEMORY_C_SRC = $(MEMORY_DIR)/memory.c
SHELL_C_SRC = $(SHELL_DIR)/shell.c
COMMANDS_C_SRC = $(SHELL_DIR)/commands.c
IDT_C_SRC = $(IDT_DIR)/idt.c
IDT_ASM_SRC = $(IDT_DIR)/idt_asm.asm

# Object files
KERNEL_OBJ = $(KERNEL_DIR)/kernel.o
VGA_OBJ = $(VGA_DIR)/vga.o
KEYBOARD_OBJ = $(KEYBOARD_DIR)/keyboard.o
MEMORY_OBJ = $(MEMORY_DIR)/memory.o
SHELL_OBJ = $(SHELL_DIR)/shell.o
COMMANDS_OBJ = $(SHELL_DIR)/commands.o
IDT_OBJ = $(IDT_DIR)/idt.o
IDT_ASM_OBJ = $(IDT_DIR)/idt_asm.o

ALL_OBJ = $(KERNEL_OBJ) $(VGA_OBJ) $(KEYBOARD_OBJ) $(MEMORY_OBJ) $(SHELL_OBJ) $(COMMANDS_OBJ) $(IDT_OBJ) $(IDT_ASM_OBJ)

# Output files
BOOT_BIN = $(BOOT_DIR)/boot.bin
KERNEL_BIN = kernel.bin
OS_IMAGE = myos.img

# Targets
.PHONY: all clr run dirs

all: dirs $(OS_IMAGE)

# Create directory structure
dirs:
	@mkdir -p $(BOOT_DIR)
	@mkdir -p $(KERNEL_DIR)
	@mkdir -p $(VGA_DIR)
	@mkdir -p $(KEYBOARD_DIR)
	@mkdir -p $(MEMORY_DIR)
	@mkdir -p $(SHELL_DIR)
	@mkdir -p $(IDT_DIR)

# Build bootloader
$(BOOT_BIN): $(BOOT_SRC)
	@echo "Building bootloader..."
	$(ASM) $(ASMFLAGS) $(BOOT_SRC) -o $(BOOT_BIN)

# Compile kernel.c
$(KERNEL_OBJ): $(KERNEL_C_SRC)
	@echo "Compiling kernel.c..."
	$(CC) $(CFLAGS) $(KERNEL_C_SRC) -o $(KERNEL_OBJ)

# Compile vga.c
$(VGA_OBJ): $(VGA_C_SRC)
	@echo "Compiling vga.c..."
	$(CC) $(CFLAGS) $(VGA_C_SRC) -o $(VGA_OBJ)

# Compile keyboard.c
$(KEYBOARD_OBJ): $(KEYBOARD_C_SRC)
	@echo "Compiling keyboard.c..."
	$(CC) $(CFLAGS) $(KEYBOARD_C_SRC) -o $(KEYBOARD_OBJ)

# Compile memory.c
$(MEMORY_OBJ): $(MEMORY_C_SRC)
	@echo "Compiling memory.c..."
	$(CC) $(CFLAGS) $(MEMORY_C_SRC) -o $(MEMORY_OBJ)

# Compile shell.c
$(SHELL_OBJ): $(SHELL_C_SRC)
	@echo "Compiling shell.c..."
	$(CC) $(CFLAGS) $(SHELL_C_SRC) -o $(SHELL_OBJ)

# Compile commands.c
$(COMMANDS_OBJ): $(COMMANDS_C_SRC)
	@echo "Compiling commands.c..."
	$(CC) $(CFLAGS) $(COMMANDS_C_SRC) -o $(COMMANDS_OBJ)

# Compile idt.c
$(IDT_OBJ): $(IDT_C_SRC)
	@echo "Compiling idt.c..."
	$(CC) $(CFLAGS) $(IDT_C_SRC) -o $(IDT_OBJ)

# Assemble idt_asm.asm
$(IDT_ASM_OBJ): $(IDT_ASM_SRC)
	@echo "Assembling idt_asm.asm..."
	$(ASM) -f elf $(IDT_ASM_SRC) -o $(IDT_ASM_OBJ)

# Link kernel
$(KERNEL_BIN): $(ALL_OBJ)
	@echo "Linking kernel..."
	$(LD) $(LDFLAGS) $(ALL_OBJ) -o kernel.elf
	objcopy -O binary kernel.elf $(KERNEL_BIN)

# Create OS image
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	@echo "Creating OS image..."
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(OS_IMAGE)
	@truncate -s 1M $(OS_IMAGE)
	@echo "Build complete! OS image: $(OS_IMAGE)"

# Run in QEMU with no reboot
run: $(OS_IMAGE)
	@echo "Starting QEMU (no reboot/shutdown)..."
	qemu-system-i386 -fda $(OS_IMAGE) -no-reboot -no-shutdown

# Run in QEMU with debugging
debug: $(OS_IMAGE)
	@echo "Starting QEMU with debugger..."
	qemu-system-i386 -fda $(OS_IMAGE) -s -S

# Clean build artifacts
clr:
	@echo "Cleaning build artifacts..."	
	rm -f $(ALL_OBJ) $(BOOT_BIN) kernel.elf $(KERNEL_BIN) $(OS_IMAGE)
	@echo "Clean complete!"

# Help target
help:
	@echo "MyOS Build System"
	@echo "================="
	@echo ""
	@echo "Targets:"
	@echo "  all    - Build the OS image (default)"
	@echo "  run    - Build and run in QEMU (no reboot/shutdown)"
	@echo "  debug  - Build and run in QEMU with GDB support"
	@echo "  clr    - Remove all build artifacts"
	@echo "  help   - Show this help message"
