org 0x7C00
bits 16

mov ax, 0
mov ds, ax
cli

main:
    mov al, 0x13
    int 0x10

    mov ax, 0xA000
    mov es, ax
    mov al, 0

print_screen:
    mov cl, al
    push cx
    mov cx, 64000
    mov di, 0

    print_pixel:
        cmp cx, 0
        je repeat
        mov dx, 0x03C8
        inc al
        push ax
        out dx, al

        mov dx, 0x03C9

        out dx, al 
        mov [es:di], al
        inc di
        dec cx

        out dx, al 
        mov [es:di], al
        inc di
        dec cx 

        out dx, al 
        mov [es:di], al
        inc di
        dec cx

        pop ax

        jmp print_pixel

    repeat:
        pop ax
        inc al
        jmp print_screen

times 510 - ($ - $$) db 0
dw 0xAA55