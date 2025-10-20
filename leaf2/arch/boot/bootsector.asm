; LeafOS single-stage bootsector
; - prints B, loads kernel.bin at 0x10000
; - enters protected mode, jumps to kernel

[org 0x7C00]
[bits 16]

cli
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7C00
sti

; save BIOS drive
mov [boot_drive], dl

; --- show 'B' ---
mov ax, 0xB800
mov ds, ax
mov byte [0x0000], 'B'
mov byte [0x0001], 0x0A

; --- load kernel.bin from sector 2 ---
mov ax, 0x1000        ; ES = 0x1000 → 0x10000 physical
mov es, ax
xor bx, bx
mov ah, 0x02          ; BIOS read sectors
mov al, 10            ; sectors to read (adjust to kernel size)
mov ch, 0             ; cylinder
mov cl, 2             ; sector 2
mov dh, 0             ; head
mov dl, [boot_drive]  ; boot drive
int 0x13
jc disk_error

; --- show 'L' ---
mov ax, 0xB800
mov ds, ax
mov byte [0x0002], 'L'
mov byte [0x0003], 0x0E

; --- enable A20 ---
in al, 0x92
or al, 2
out 0x92, al

; --- GDT setup ---
lgdt [gdt_descriptor]

; enter protected mode
mov eax, cr0
or eax, 1
mov cr0, eax
jmp CODE_SEG:pm_entry

disk_error:
    mov ax, 0xB800
    mov ds, ax
    mov byte [0x0004], 'E'
    mov byte [0x0005], 0x0C
    hlt
    jmp $

; ---------------- GDT ----------------
align 8
gdt_start:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ 0x08
DATA_SEG equ 0x10

boot_drive db 0

; ------------- protected mode -------------
[bits 32]
pm_entry:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x9FC00

    ; show 'P' in VGA
    mov dword [0xB8000 + 6], 0x0B0050   ; 'P' light cyan

    ; jump to kernel entry at 0x10000
    jmp 0x0010000

times 510 - ($ - $$) db 0
dw 0xAA55
