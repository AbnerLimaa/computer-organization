org 0x7C00
bits 16

main:
    mov ax, 0
    mov ds, ax
    cli

    mov ah, 0
    mov al, 0x13
    int 0x10

    int 0x13
    mov ah, 0x02
    mov al, 32
    mov cl, 2
    mov ch, 0
    mov dh, 0
    mov bx, 0x7E00
    int 0x13

    mov ax, 0xA000
    mov es, ax
    mov di, 0
    mov si, bx

    call print_imagem
    hlt

print_imagem:
    push di
    push si
    mov cx, 0

    .print_pixel:
        mov al, [ds:si]
        mov [es:di], al
        inc si
        inc di
        inc cx
        cmp cx, 16000
        jne .print_pixel

    pop si
    pop di
    ret

times 510 - ($ - $$) db 0
dw 0xAA55