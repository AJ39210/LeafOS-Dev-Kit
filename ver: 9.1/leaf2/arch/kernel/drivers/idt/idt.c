#include "idt.h"

#define IDT_ENTRIES 256

// IDT entries array
static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idt_ptr;

// External assembly interrupt handlers
extern void isr_stub_0(void);
extern void isr_stub_1(void);
extern void isr_stub_generic(void);

// Set an IDT gate
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

// Load IDT using assembly
static inline void idt_load(uint32_t idt_ptr_addr) {
    __asm__ __volatile__("lidt (%0)" : : "r"(idt_ptr_addr));
}

// Default interrupt handler
void default_isr_handler(void) {
    // Do nothing - just return
    // This prevents triple faults from unhandled interrupts
}

void idt_init(void) {
    // Setup IDT pointer
    idt_ptr.limit = sizeof(idt_entry_t) * IDT_ENTRIES - 1;
    idt_ptr.base = (uint32_t)&idt;
    
    // Clear IDT
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    // Set up stub handlers for all interrupts
    // Using a generic stub that just returns
    uint32_t stub_addr = (uint32_t)isr_stub_generic;
    
    for (int i = 0; i < IDT_ENTRIES; i++) {
        // 0x8E = present, ring 0, 32-bit interrupt gate
        idt_set_gate(i, stub_addr, 0x08, 0x8E);
    }
    
    // Load the IDT
    idt_load((uint32_t)&idt_ptr);
    
    // Enable interrupts
    __asm__ __volatile__("sti");
}