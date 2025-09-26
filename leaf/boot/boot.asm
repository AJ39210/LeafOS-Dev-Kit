; boot.asm -- 512 bytes, org 0x7C00
org 0x7c00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; save BIOS drive number from DL for later int13 read
    mov [boot_drive], dl

    ; load 1 sector (kernel) from sector 2 (CHS: head 0, track 0, sector 2)
    xor ax, ax
    mov bx, 0x8000        ; ES:BX -> 0x0000:0x8000 (where we'll load the kernel)
    mov es, ax

    mov ah, 0x02          ; BIOS read sectors
    mov al, 0x01          ; read 1 sector
    mov ch, 0x00          ; cylinder 0
    mov cl, 0x02          ; sector 2 (first sector is 1 = boot, second is 2 = kernel)
    mov dh, 0x00          ; head 0
    mov dl, [boot_drive]  ; drive number (from BIOS)
    int 0x13
    jc disk_error         ; if carry set, error

    ; far jump to loaded kernel at 0x0000:0x8000
    jmp 0x0000:0x8000

disk_error:
    ; error: blink/hang
    mov si, err_msg
.showerr:
    call print_string
    jmp .showerr

; simple print_string (uses BIOS teletype int 0x10 AH=0x0E)
print_string:
    pusha
.nextchar:
    lodsb
    or al, al
    jz .ret
    mov ah, 0x0e
    int 0x10
    jmp .nextchar
.ret:
    popa
    ret

boot_drive db 0
err_msg db "Disk read error",0

; pad to 510 bytes, then boot signature 0x55AA
times 510-($-$$) db 0
dw 0xAA55
