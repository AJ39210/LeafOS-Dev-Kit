; kernel.asm -- single 512-byte sector
; ORG 0x8000 (loaded by bootloader at 0000:0x8000)
org 0x8000

; --- setup segments ---
    cli
    xor ax,ax
    mov ds,ax
    mov es,ax
    sti

; -------------------------
; ENTRY / MAIN SHELL LOOP
; -------------------------
main_loop:
    mov si, prompt
    call print_str

    ; read input into input_buf (read_line returns with DI pointing after string)
    mov di, input_buf
    call read_line

    ; if empty line -> loop
    cmp byte [input_buf], 0
    je main_loop

    ; check "echo " (starts with "echo " -> print remainder)
    mov si, cmd_echo_sp    ; "echo "
    mov di, input_buf
    call strncmp5
    cmp ax,0
    je do_echo

    ; check "clear"
    mov si, cmd_clear
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_clear

    ; check "setup"
    mov si, cmd_setup
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_setup

    ; check "reboot"
    mov si, cmd_reboot
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_reboot

    ; check "panic" (dev)
    mov si, cmd_panic
    mov di, input_buf
    call strcmp
    cmp ax,0
    je kernel_panic

    ; unknown
    mov si, unknown
    call print_str
    jmp main_loop

; -------------------------
; COMMANDS
; -------------------------
do_echo:
    ; print after "echo "
    lea si, [input_buf + 5]
    call print_str
    call newline
    jmp main_loop

do_clear:
    call cls
    jmp main_loop

do_setup:
    ; ask username
    mov si, setup_user
    call print_str
    mov di, username_buf
    call read_line
    ; save username
    mov si, username_buf
    mov di, saved_user
    call strcpy

    ; ask password
    mov si, setup_pass
    call print_str
    mov di, password_buf
    call read_line
    ; save password
    mov si, password_buf
    mov di, saved_pass
    call strcpy

    mov si, setup_done
    call print_str
    call newline
    jmp main_loop

do_reboot:
    ; warm reboot
    jmp 0FFFFh:0000h

; -------------------------
; MENU (triggered by ESC or "menu")
; password protected
; -------------------------
do_menu:
    ; prompt for password
    mov si, menu_pass_prompt
    call print_str
    mov di, input_buf
    call read_line

    ; compare with saved_pass
    mov si, saved_pass
    mov di, input_buf
    call strcmp
    cmp ax,0
    jne menu_bad_pass

    ; show menu
    mov si, menu_header
    call print_str
    call newline
    mov si, menu_option1
    call print_str
    call newline
    mov si, menu_option2
    call print_str
    call newline
    mov si, menu_option3
    call print_str
    call newline

    mov si, menu_choice_prompt
    call print_str
    mov di, input_buf
    call read_line

    ; check "1"
    mov si, str_one
    mov di, input_buf
    call strcmp
    cmp ax,0
    je menu_continue

    ; check "2"
    mov si, str_two
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_reboot

    ; check "3" -> Next -> Please wait... then panic shell
    mov si, str_three
    mov di, input_buf
    call strcmp
    cmp ax,0
    je menu_next

    ; invalid -> back
    mov si, unknown
    call print_str
    jmp main_loop

menu_bad_pass:
    mov si, bad_pass
    call print_str
    jmp main_loop

menu_continue:
    mov si, cont_msg
    call print_str
    jmp main_loop

menu_next:
    mov si, wait_msg
    call print_str
    call newline
    ; fall into panic shell
    jmp kernel_panic

; -------------------------
; KERNEL PANIC SHELL
; -------------------------
kernel_panic:
    mov si, panic_msg
    call print_str
    call newline

panic_loop:
    mov si, emerg_prompt
    call print_str
    mov di, input_buf
    call read_line

    ; wipe cache
    mov si, cmd_wipe_cache
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_wipe_cache

    ; wipe data (factory reset)
    mov si, cmd_wipe_data
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_wipe_data

    ; info
    mov si, cmd_info
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_info

    ; help
    mov si, cmd_help
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_help

    mov si, unknown
    call print_str
    jmp panic_loop

do_wipe_cache:
    mov si, cache_msg
    call print_str
    call newline
    jmp panic_loop

do_wipe_data:
    mov si, data_msg
    call print_str
    call newline
    ; clear saved credentials (factory reset)
    lea di, [saved_user]
    mov cx, 32
    xor al,al
    rep stosb
    lea di, [saved_pass]
    mov cx, 32
    rep stosb
    jmp panic_loop

do_info:
    mov si, info_msg
    call print_str
    call newline
    jmp panic_loop

do_help:
    mov si, help_msg
    call print_str
    call newline
    jmp panic_loop

; -------------------------
; HELPERS
; -------------------------
; print_str: DS:SI -> null terminated
print_str:
    pusha
.ps_loop:
    lodsb
    or al, al
    jz .ps_done
    mov ah, 0x0E
    int 0x10
    jmp .ps_loop
.ps_done:
    popa
    ret

; newline (CR LF)
newline:
    pusha
    mov ah,0x0E
    mov al,0x0D
    int 0x10
    mov al,0x0A
    int 0x10
    popa
    ret

; cls - clear screen
cls:
    mov ax,0x0600
    mov bh,0x07
    mov cx,0x0000
    mov dx,0x184F
    int 0x10
    ret

; read_line: reads into DS:DI, supports backspace and ESC (0x1B)
; on ESC -> call do_menu and return with empty buffer
read_line:
    pusha
    mov bx, di        ; save buffer start
.rl:
    mov ah,0
    int 0x16
    cmp al,0x1B       ; ESC
    je .esc
    cmp al,0x0D       ; Enter
    je .done
    cmp al,0x08       ; Backspace
    je .back
    ; store and echo
    stosb
    mov ah,0x0E
    int 0x10
    jmp .rl

.back:
    ; if DI > buffer start then backspace
    cmp di, bx
    je .rl
    dec di
    ; move cursor back, space, back
    mov ah,0x0E
    mov al,0x08
    int 0x10
    mov al,' '
    int 0x10
    mov al,0x08
    int 0x10
    jmp .rl

.esc:
    ; terminate buffer with zero then call menu handler
    mov al,0
    stosb
    popa
    call do_menu
    ret

.done:
    ; terminate string, echo CRLF
    mov al,0
    stosb
    mov ah,0x0E
    mov al,0x0D
    int 0x10
    mov al,0x0A
    int 0x10
    popa
    ret

; strcmp: compare DS:SI (pattern) with DS:DI (input). AX=0 if equal, AX=1 if not.
; both null-terminated
strcmp:
    push si
    push di
.sc_loop:
    mov al, [si]
    mov bl, [di]
    cmp al, bl
    jne .ne
    cmp al, 0
    je .eq
    inc si
    inc di
    jmp .sc_loop
.ne:
    mov ax,1
    jmp .sc_ret
.eq:
    xor ax,ax
.sc_ret:
    pop di
    pop si
    ret

; strcpy: copy DS:SI -> DS:DI (including null)
strcpy:
    pusha
.copy_loop:
    lodsb
    stosb
    or al, al
    jnz .copy_loop
    popa
    ret

; strncmp5: compare first 5 chars (used for "echo ")
; AX=0 if equal, 1 otherwise
strncmp5:
    push si
    push di
    mov cx,5
.sn_loop:
    mov al, [si]
    mov bl, [di]
    cmp al, bl
    jne .sn_ne
    inc si
    inc di
    loop .sn_loop
    xor ax,ax
    jmp .sn_ret
.sn_ne:
    mov ax,1
.sn_ret:
    pop di
    pop si
    ret

; -------------------------
; DATA (origin 0x8000)
; -------------------------
prompt      db "> ",0
unknown     db "Unknown command",0

cmd_echo_sp db "echo ",0
cmd_clear   db "clear",0
cmd_setup   db "setup",0
cmd_reboot  db "reboot",0
cmd_panic   db "panic",0
cmd_help    db "help",0

setup_user  db "Enter username: ",0
setup_pass  db "Enter password: ",0
setup_done  db "[Setup complete]",0

menu_pass_prompt db "Password: ",0
menu_header db "=== MENU ===",0
menu_option1 db "1) Continue",0
menu_option2 db "2) Exit (reboot)",0
menu_option3 db "3) Next",0
menu_choice_prompt db "Choice: ",0
bad_pass    db "Bad password",0
cont_msg    db "[Continuing...]",0
wait_msg    db "Please wait...",0

str_one db "1",0
str_two db "2",0
str_three db "3",0

panic_msg   db "!! Kernel Panic !!",0
emerg_prompt db "kernel> ",0

cmd_wipe_cache db "wipe cache",0
cmd_wipe_data  db "wipe data",0
cmd_info       db "info",0
help_msg       db "Commands: wipe cache, wipe data, info, help",0

cache_msg    db "[Cache cleared]",0
data_msg     db "[Data wiped (factory reset)]",0
info_msg     db "[Debug: LeafOS test build v0.1]",0

username_buf times 32 db 0
password_buf times 32 db 0
saved_user  times 32 db 0
saved_pass  times 32 db 0

input_buf    times 128 db 0
