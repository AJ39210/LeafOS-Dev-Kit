; idt_asm.asm - Interrupt handler stubs

[BITS 32]

global isr_stub_generic

; Generic interrupt stub - just returns
isr_stub_generic:
    iret