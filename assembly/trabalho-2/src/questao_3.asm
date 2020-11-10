org 0X7C00
bits 16

mov ax, 0
mov ds, ax
cli

main:
    call texto
    hlt

texto:
    mov cx, 0

    ler:
        mov ah, 0
        int 0x16
        cmp al, 13
        je escrever
        push ax
        inc cx
        mov ah, 0x0E
        int 0x10
        jmp ler
    
    escrever:
            mov ah, 0x0E
            mov al, 13
            int 0x10
            mov al, 10
            int 0x10

            frase:
                cmp cx, 0
                je fim_texto
                dec cx
                pop ax
                mov ah, 0x0E
                int 0x10
                jmp frase

    fim_texto:
        ret

times 510 - ($ - $$) db 0
dw 0xAA55