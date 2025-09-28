OUTPUT_FORMAT("binary")
ENTRY(start)

SECTIONS
{
    . = 0x8000;          /* stage2 load address */
    .text : { *(.text) } /* stage2 + kernel text */
    .data : { *(.data) }
    .bss  : { *(.bss)  }
}
