; boot/idt_flush.asm - Flush IDT

section .note.GNU-stack noalloc noexec nowrite progbits

global idt_flush

idt_flush:
    mov eax, [esp+4]  ; Get pointer to IDT
    lidt [eax]        ; Load IDT
    ret