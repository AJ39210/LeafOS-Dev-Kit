[BITS 16]
[ORG 0x7C00]

start:
    xor ax, ax
    mov ds, ax
    mov es, ax

    mov si, msg
    call print_string

    mov bx, 0x8000         ; Load kernel to 0x8000
    mov ah, 0x02       
    mov al, 2              ; Number of sectors to read
    mov ch, 0             
    mov cl, 2              ; Sector starts at 1
    mov dh, 0              
    mov dl, 0x00           ; Drive (floppy)
    int 0x13
    jc disk_error

    jmp 0x0000:0x8000      

disk_error:
    mov si, err
    call print_string
    jmp $

print_string:
    mov ah, 0x0E
.next:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .next
.done:
    ret

msg db "Booting kernel...", 0
err db "Disk read error!", 0

times 510-($-$$) db 0
dw 0xAA55
