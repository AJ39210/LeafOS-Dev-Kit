[BITS 16]
[ORG 0x8000]

start:
    mov si, hello
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

hello db "Hello from kernel!", 0
