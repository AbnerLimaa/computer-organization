org 0x7C00
bits 16

mov ax, 0
mov ds, ax
cli

mov al, 0x13
int 0x10

main:
    mov ax, 0x7E00
    call get_tecla

    mov ax, 0x7E00
    call pinta_tela

    jmp main

get_tecla:
    push ax
    mov di, ax

    mov ah, 0x00
    int 0x16
    mov [ds:di], al

    pop ax
    ret

pinta_tela:
    push ax
    push cx
    push si
    push di
    mov si, ax
    mov cx, 64000
    mov ax, 0xA000
    mov es, ax
    mov di, 0

    pinta_pixel:
        cmp cx, 0
        je fim_pinta_tela
        mov ax, [si]
        mov [es:di], ax
        inc di
        dec cx
        jmp pinta_pixel
    
    fim_pinta_tela:
        pop di
        pop si
        pop cx
        pop ax
        ret

times 510 - ($ - $$) db 0
dw 0xAA55