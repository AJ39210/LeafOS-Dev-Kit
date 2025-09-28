; boot.asm - Stage1 bootloader
org 0x7C00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    sti

    ; Load stage2 (sector 2) into 0x8000
    mov bx, 0x8000       ; destination
    mov dh, 0
    mov dl, 0x80         ; first HDD
    mov ch, 0
    mov cl, 2            ; sector 2
    mov ah, 0x02         ; read sectors
    mov al, 1            ; read 1 sector
    int 0x13
    jc disk_error

    jmp 0x8000           ; jump to stage2

disk_error:
    hlt

times 510-($-$$) db 0
dw 0xAA55
