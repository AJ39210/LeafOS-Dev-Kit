section .initrd
global _initrd_entry

_initrd_entry:
    ; Just a stub — maybe print something or set up a jump
    ; You could even call a function in filesystem.asm
    ret
