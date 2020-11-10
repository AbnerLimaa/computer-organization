org 0X7C00
bits 16

mov ax, 0
mov ds, ax
cli

main:    
    int 0x13
    mov ah, 0x02
    mov al, 1
    mov cl, 2
    mov ch, 0
    mov dh, 0
    mov bx, 0x7E00
    int 0x13

    mov di, 0x8000
    call decodificar

    mov si, 0x8000
    call print

    hlt

decodificar:
    push ax
    push bx
    push di

    mov bx, 0x7E00
    mov si, matricula 

    ler_byte: 
        lodsb
        cmp ax, 10
        je zerar_si
        cmp bx, 0x8000
        je fim_decodificar
        mov dx, [bx]
        sub dx, ax
        mov [di], dx
        inc di
        inc bx
        jmp ler_byte
    
    zerar_si:
        mov si, matricula
        jmp ler_byte

    fim_decodificar:
        pop di
        pop bx
        pop ax
        ret

print:
    push si
    mov ah, 0x0E

    ler_texto:
        lodsb
        or al, al
        jz fim_print
        int 0x10
        jmp ler_texto

    fim_print:
        pop si
        ret

matricula: db 3, 9, 8, 0, 6, 7, 10
           
times 510 - ($ - $$) db 0
dw 0xAA55