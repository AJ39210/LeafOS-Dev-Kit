; boot/boot.asm  (MBR, 512 bytes)
org 0x7C00

cli
xor ax, ax
mov ds, ax
mov es, ax
sti

; load stage2 (from sector 2) to 0x8000
mov bx, 0x8000
mov dh, 0
mov dl, 0x80    ; first HDD
mov ch, 0
mov cl, 2       ; sector 2 (right after MBR)
mov ah, 0x02
mov al, 1       ; read 1 sector (stage2 fits in 1 sector for this minimal example)
int 0x13
jc disk_error

jmp 0x8000      ; jump to stage2

disk_error:
    hlt

times 510-($-$$) db 0
dw 0xAA55
