; boot.asm - Simple bootloader that loads our kernel
bits 16
org 0x7c00

boot:
    ; Set up segment registers
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti

    ; Load kernel from disk
    mov bx, 0x1000      ; Load kernel at 0x1000
    mov ah, 0x02        ; Read sectors function
    mov al, 40          ; Number of sectors to read (increased)
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Start from sector 2
    mov dh, 0           ; Head 0
    int 0x13            ; BIOS interrupt

    jc disk_error       ; Jump if error

    ; Switch to protected mode
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode

disk_error:
    mov si, error_msg
    call print_string
    hlt

print_string:
    lodsb
    or al, al
    jz done
    mov ah, 0x0e
    int 0x10
    jmp print_string
done:
    ret

error_msg db 'Disk error!', 0

bits 32
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000

    ; Jump to kernel entry point
    call 0x1000
    
    ; If kernel returns (it shouldn't), halt here
    cli
hang:
    hlt
    jmp hang

; GDT
gdt_start:
    dq 0x0

gdt_code:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0

gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510-($-$$) db 0
dw 0xaa55