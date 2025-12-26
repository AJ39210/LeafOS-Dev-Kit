/* linker.ld - Linker script for MyOS */

ENTRY(_start)

SECTIONS
{
    /* Start at 1MB as per multiboot spec */
    . = 1M;

    /* Multiboot header MUST be in first 8KB and FIRST in file */
    .multiboot ALIGN(4) :
    {
        KEEP(*(.multiboot))
    }

    .text ALIGN(4K) :
    {
        *(.text)
    }

    .rodata ALIGN(4K) :
    {
        *(.rodata)
    }

    .data ALIGN(4K) :
    {
        *(.data)
    }

    .bss ALIGN(4K) :
    {
        *(COMMON)
        *(.bss)
    }
}   