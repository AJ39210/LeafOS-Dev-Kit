org 0x7c00
start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    mov [boot_drive], dl

    ; load 4 sectors into 0x0000:0x8000
    xor ax, ax
    mov es, ax
    mov bx, 0x8000
    mov ah, 0x02
    mov al, 0x04        ; read 4 sectors
    mov ch, 0x00
    mov cl, 0x02
    mov dh, 0x00
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    jmp 0x0000:0x8000

disk_error:
    mov si, err
    call print_string
    jmp $

print_string:
    pusha
.next:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0e
    int 0x10
    jmp .next
.done:
    popa
    ret

boot_drive db 0
err db "Disk error",0
times 510-($-$$) db 0
dw 0xAA55
