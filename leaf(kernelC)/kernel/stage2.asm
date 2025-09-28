; stage2.asm - 32-bit protected mode bootloader for ELF32
; Assemble with: nasm -f elf32 kernel/stage2.asm -o build/stage2.o

[bits 16]

; -----------------------
; Real mode section (load kernel)
; -----------------------
start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; load 20 sectors of kernel to 0x10000
    mov ax, 0x1000        ; segment for ES
    mov es, ax
    xor bx, bx            ; offset
    mov ah, 0x02          ; BIOS read sectors
    mov al, 20            ; number of sectors
    mov ch, 0
    mov cl, 3
    mov dh, 0
    mov dl, 0x80          ; first HDD
    int 0x13

[bits 32]

; -----------------------
; Protected mode setup
; -----------------------
extern kmain             ; C kernel entry

; Minimal GDT
gdt_start:
    dq 0x0000000000000000 ; null descriptor
    dq 0x00CF9A000000FFFF ; code segment
    dq 0x00CF92000000FFFF ; data segment
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; Enable protected mode
protected_mode_start:
    cli

    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1           ; enable PE bit
    mov cr0, eax

    ; Far jump to flush prefetch and load CS
    jmp 0x08:pm_entry

pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9FC00

    ; Call C kernel
    call kmain

    hlt
