; kernel/stage2.asm  (flat binary, assembled with: nasm -f bin kernel/stage2.asm -o build/stage2.bin)
org 0x8000
[bits 16]

; ---------------------------
; Real mode loader: load kernel to 0x10000
; ---------------------------
start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; print small debug string (optional) using BIOS teletype
    mov si, msg
    call print_string

    ; Set ES:BX to 0x10000 (segment 0x1000, offset 0)
    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    ; Read 20 sectors from disk starting at sector 3 into ES:BX (kernel size may vary)
    mov ah, 0x02       ; BIOS read sectors
    mov al, 20         ; number of sectors to read (adjust if kernel bigger/smaller)
    mov ch, 0
    mov cl, 3          ; start at sector 3
    mov dh, 0
    mov dl, 0x80
    int 0x13
    jc disk_error

    ; prepare GDT descriptor address in memory (we placed GDT after code)
    ; now enable protected mode using GDT below
    lgdt [gdt_descriptor]

    ; enable A20 line is skipped (qemu usually okay). If needed, implement A20 enable here.

    ; set PE bit in CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; far jump into protected mode code (selector 0x08 is code descriptor)
    jmp 0x08:pm_entry

disk_error:
    hlt

; ---------------------------
; 16-bit print routine using BIOS teletype (optional)
; ---------------------------
print_string:
    mov ah, 0x0E
.nextch:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .nextch
.done:
    ret

msg db 'stage2: loading kernel...', 0

; ---------------------------
; GDT (placed in-stage)
; Descriptor layout uses 8-byte entries (we define three entries)
; ---------------------------
; Align data on next addresses (GDT is referenced by lgdt)
gdt_start:
    dq 0x0000000000000000        ; null descriptor
    dq 0x00CF9A000000FFFF        ; code segment: base=0, limit=0xFFFFF, 32-bit code, executable, readable
    dq 0x00CF92000000FFFF        ; data segment: base=0, limit=0xFFFFF, 32-bit data, writable
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1   ; size (limit)
    dd gdt_start                 ; base (address of GDT)

; ---------------------------
; Now in protected mode (32-bit)
; ---------------------------
[bits 32]
pm_entry:
    ; set data segments to data selector (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; set up stack
    mov esp, 0x9FC00

    ; Jump to kernel entry at physical 0x10000 (kernel was loaded there)
    mov eax, 0x00010000
    jmp eax

    ; never returns
    hlt

