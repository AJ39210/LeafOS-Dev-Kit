; kernel.asm -- single 512-byte sector, origin 0x8000
org 0x8000

start:
    ; loop forever: prompt -> read line -> print "Unknown command" -> repeat
main_loop:
    mov si, prompt
    call print_string

    ; read a line, echoing characters (no history, no edit except ignoring backspace)
    xor di, di         ; (not used, we don't store the line)
.read_loop:
    mov ah, 0x00
    int 0x16           ; BIOS keyboard: returns character in AL
    cmp al, 0x0d       ; Enter?
    je .line_done
    cmp al, 0x08       ; Backspace?
    je .bs_handling
    ; echo char
    mov ah, 0x0e
    int 0x10
    jmp .read_loop

.bs_handling:
    ; ignore backspace (could implement delete echo if desired)
    jmp .read_loop

.line_done:
    ; print CR+LF
    mov al, 0x0d
    mov ah, 0x0e
    int 0x10
    mov al, 0x0a
    mov ah, 0x0e
    int 0x10

    ; print Unknown command
    mov si, unknown_msg
    call print_string

    jmp main_loop

; print_string: DS:SI -> null-terminated string, uses BIOS teletype
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

prompt db "> ",0
unknown_msg db "Unknown command",0

; pad to 512 bytes (kernel must be exactly one sector for our simple write)
times 512 - ($ - $$) db 0
