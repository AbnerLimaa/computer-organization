org 0x7c00
bits 16

mov ax, 0x00
mov ds, ax 
cli

main: 
    mov al, 0x13
    int 0x10

    mov ax, 0xA000
    mov es, ax

    mov cx, 64000
    mov bx, 0

    .print_pixel:
        mov di, bx
        mov [es:di], byte 40
        inc bx
        dec cx
        jnz .print_pixel

    hlt

times 510 - ($-$$) db 0
dw 0xAA55