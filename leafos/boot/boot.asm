; boot/boot.asm - Multiboot-compliant bootloader entry point
; This file sets up the initial environment and jumps to the kernel

MBALIGN  equ 1 << 0
MEMINFO  equ 1 << 1
FLAGS    equ MBALIGN | MEMINFO
MAGIC    equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

; Multiboot header - MUST be in first 8KB of file
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
    resb 16384  ; 16 KB stack
stack_top:

section .text
global _start:function (_start.end - _start)
extern kernel_main

_start:
    ; Set up stack
    mov esp, stack_top

    ; Push multiboot info structure pointer
    push ebx
    ; Push multiboot magic number
    push eax

    ; Call kernel main function
    call kernel_main

    ; If kernel returns, halt the system
    cli
.hang:
    hlt
    jmp .hang
.end: