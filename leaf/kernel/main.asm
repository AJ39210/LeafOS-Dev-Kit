; LeafOS Kernel.asm (full, with set command)
org 0x8000

cli
xor ax,ax
mov ds,ax
mov es,ax
sti

; -------------------------
; MAIN SHELL LOOP
; -------------------------
main_loop:
    mov si, prompt_buf
    call print_str

    mov di, input_buf
    call read_line

    cmp byte [input_buf], 0
    je main_loop

    ; echo
    mov si, cmd_echo_sp
    mov di, input_buf
    call strncmp5
    cmp ax, 0
    je do_echo

    ; clear
    mov si, cmd_clear
    mov di, input_buf
    call strcmp
    cmp ax, 0
    je do_clear

    ; setup
    mov si, cmd_setup
    mov di, input_buf
    call strcmp
    cmp ax, 0
    je do_setup

    ; reboot
    mov si, cmd_reboot
    mov di, input_buf
    call strcmp
    cmp ax, 0
    je do_reboot

    ; about
    mov si, cmd_about
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_about

    ; panic
    mov si, cmd_panic
    mov di, input_buf
    call strcmp
    cmp ax, 0
    je kernel_panic

    ; set
    mov si, cmd_set
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_set

    mov si, unknown
    call print_str
    call newline
    jmp main_loop

; -------------------------
; COMMAND HANDLERS
; -------------------------
do_echo:
    lea si, [input_buf + 5]
    call print_str
    call newline
    jmp main_loop

do_clear:
    call cls
    jmp main_loop

do_setup:
    cmp byte [setup_done_flag], 1
    je setup_already_done

    mov si, setup_user
    call print_str
    mov di, username_buf
    call read_line
    mov si, username_buf
    mov di, saved_user
    call strcpy

    mov si, setup_pass
    call print_str
    mov di, password_buf
    call read_line
    mov si, password_buf
    mov di, saved_pass
    call strcpy

    mov si, setup_pc_name
    call print_str
    mov di, pc_name
    call read_line

    mov si, setup_pc_arch
    call print_str
    mov di, pc_arch
    call read_line

    mov si, setup_pc_proc
    call print_str
    mov di, pc_proc
    call read_line

    mov si, setup_pc_ram
    call print_str
    mov di, pc_ram
    call read_line

    mov si, setup_pc_osver
    call print_str
    mov di, pc_osver
    call read_line

    mov byte [setup_done_flag], 1

    mov si, setup_done
    call print_str
    call newline
    jmp main_loop

setup_already_done:
    mov si, setup_locked_msg
    call print_str
    call newline
    jmp main_loop

do_reboot:
    jmp 0FFFFh:0000h

do_about:
    mov si, about_msg
    call print_str
    call newline

    lea si, [saved_pass+30]
    mov cx,2
.ab_loop:
    lodsb
    mov ah,0x0E
    int 0x10
    loop .ab_loop
    call newline

    mov si, pc_name
    call print_str
    call newline
    mov si, pc_arch
    call print_str
    call newline
    mov si, pc_proc
    call print_str
    call newline
    mov si, pc_ram
    call print_str
    call newline
    mov si, pc_osver
    call print_str
    call newline
    jmp main_loop

; -------------------------
; SET COMMAND
; -------------------------
do_set:
    mov si, set_prompt_msg
    call print_str
    mov di, input_buf
    call read_line

    ; Copy input_buf to prompt_buf
    mov si, input_buf
    mov di, prompt_buf
    call strcpy

    ; Optional: change color if format color=XX
    mov si, prompt_buf
    cmp byte [si], 'c'
    jne .skip_color
    cmp byte [si+1], 'o'
    jne .skip_color
    cmp byte [si+2], 'l'
    jne .skip_color
    cmp byte [si+3], 'o'
    jne .skip_color
    cmp byte [si+4], 'r'
    jne .skip_color
    cmp byte [si+5], '='
    jne .skip_color
    mov al, [si+6]
    mov [color_byte], al
.skip_color:
    call newline
    jmp main_loop

; -------------------------
; ESC MENU
; -------------------------
do_menu:
    mov si, menu_pass_prompt
    call print_str
    mov di, input_buf
    call read_line

    mov si, saved_pass
    mov di, input_buf
    call strcmp
    cmp ax,0
    jne menu_bad_pass

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

    mov si, str_one
    mov di, input_buf
    call strcmp
    cmp ax,0
    je menu_continue

    mov si, str_two
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_reboot

    mov si, str_three
    mov di, input_buf
    call strcmp
    cmp ax,0
    je menu_next

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

    mov si, cmd_wipe_cache
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_wipe_cache

    mov si, cmd_wipe_data
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_wipe_data

    mov si, cmd_info
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_info

    mov si, cmd_help
    mov di, input_buf
    call strcmp
    cmp ax,0
    je do_help

    mov si, cmd_exit
    mov di, input_buf
    call strcmp
    cmp ax,0
    je panic_exit_to_main

    mov si, unknown
    call print_str
    call newline
    jmp panic_loop

panic_exit_to_main:
    mov si, exit_msg
    call print_str
    call newline
    jmp main_loop

do_wipe_cache:
    mov si, cache_msg
    call print_str
    call newline
    jmp panic_loop

do_wipe_data:
    mov si, data_msg
    call print_str
    call newline
    lea di, [saved_user]
    mov cx, 32
    xor al, al
    rep stosb
    lea di, [saved_pass]
    mov cx, 32
    rep stosb
    mov byte [setup_done_flag],0
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
print_str:
    pusha
.ps_loop:
    lodsb
    or al, al
    jz .ps_done
    mov ah, [color_byte]
    int 0x10
    jmp .ps_loop
.ps_done:
    popa
    ret

newline:
    pusha
    mov ah,0x0E
    mov al,0x0D
    int 0x10
    mov al,0x0A
    int 0x10
    popa
    ret

cls:
    mov ax,0x0600
    mov bh,0x07
    mov cx,0x0000
    mov dx,0x184F
    int 0x10
    ret

read_line:
    pusha
    mov bx, di
.rl:
    mov ah,0
    int 0x16
    cmp al,0x1B
    je .esc
    cmp al,0x0D
    je .done
    cmp al,0x08
    je .back
    stosb
    mov ah,0x0E
    int 0x10
    jmp .rl
.back:
    cmp di,bx
    je .rl
    dec di
    mov ah,0x0E
    mov al,0x08
    int 0x10
    mov al,' '
    int 0x10
    mov al,0x08
    int 0x10
    jmp .rl
.esc:
    mov al,0
    stosb
    popa
    call do_menu
    ret
.done:
    mov al,0
    stosb
    call newline
    popa
    ret

strcmp:
    push si
    push di
.sc_loop:
    mov al,[si]
    mov bl,[di]
    cmp al,bl
    jne .ne
    cmp al,0
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

strcpy:
    pusha
.copy_loop:
    lodsb
    stosb
    or al,al
    jnz .copy_loop
    popa
    ret

strncmp5:
    push si
    push di
    mov cx,5
.sn_loop:
    mov al,[si]
    mov bl,[di]
    cmp al,bl
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
; DATA
; -------------------------
prompt_buf db "Root> ",0
color_byte db 0x0E

unknown     db "Unknown command",0
cmd_echo_sp db "echo ",0
cmd_clear   db "clear",0
cmd_setup   db "setup",0
cmd_reboot  db "reboot",0
cmd_about   db "about",0
cmd_panic   db "panic",0
cmd_help    db "help",0
cmd_exit    db "exit",0
cmd_set     db "set",0

exit_msg    db "[Returning to normal shell]",0

setup_user  db "Enter username: ",0
setup_pass  db "Enter password: ",0
setup_done  db "[Setup complete]",0
setup_locked_msg db "[Setup already completed]",0
setup_done_flag db 0

setup_pc_name   db "Enter PC name: ",0
setup_pc_arch   db "Enter architecture: ",0
setup_pc_proc   db "Enter processor: ",0
setup_pc_ram    db "Enter RAM size: ",0
setup_pc_osver  db "Enter OS version: ",0

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
help_msg       db "Commands: wipe cache, wipe data, info, help, exit",0
cache_msg    db "[Cache cleared]",0
data_msg     db "[Data wiped (factory reset)]",0
info_msg     db "[Debug: LeafOS test build v0.1]",0
about_msg    db "[About your system]",0
set_prompt_msg db "Enter new prompt (or color=XX): ",0

username_buf times 32 db 0
password_buf times 32 db 0
saved_user  times 32 db 0
saved_pass  times 32 db 0
input_buf    times 128 db 0

pc_name      times 32 db 0
pc_arch      times 16 db 0
pc_proc      times 32 db 0
pc_ram       times 16 db 0
pc_osver     times 16 db 0
