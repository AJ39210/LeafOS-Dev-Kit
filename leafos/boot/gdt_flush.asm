; boot/gdt_flush.asm - Flush GDT

section .note.GNU-stack noalloc noexec nowrite progbits

global gdt_flush

gdt_flush:
    mov eax, [esp+4]  ; Get pointer to GDT
    lgdt [eax]        ; Load GDT

    mov ax, 0x10      ; 0x10 is the offset to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush   ; 0x08 is the offset to our code segment
.flush:
    ret