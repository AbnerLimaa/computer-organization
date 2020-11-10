org 0x7C00
bits 16

mov ax, 0
mov ds, ax
cli

main:
    mov ax, 0x7E00
    call get_num

    mov ax, 0x7E00
    call teste_primo

    hlt

get_num:
    push ax
    push bx
    push dx
    push di
    mov di, ax
    mov dx, 10
    mov bx, 1
    
    ler_char:
        mov ah, 0x0
        int 0x16
        cmp al, 13
        je fim_get_num
        mov ah, 0x0E
        int 0x10
        mov ah, 0
        sub ax, 48
        imul ax, bx
        imul bx, dx
        add [di], al
        jmp ler_char
    
    fim_get_num:
        pop di
        pop dx
        pop bx
        pop ax
        ret

teste_primo:
    push ax
    push cx
    push si

    mov si, ax
    mov cx, 2

    div_num:
        mov ax, [si]        
        cmp cx, ax
        je fim_primo
        div cl
        cmp ah, 0 
        je fim_nao_primo
        inc cx
        jmp div_num

    fim_primo:
        mov ax, primo
        call print
        jmp fim_teste_primo

    fim_nao_primo:
        mov ax, nao_primo
        call print
        jmp fim_teste_primo

    fim_teste_primo:
        pop si
        pop cx
        pop ax
        ret
        
print:
    push ax
    push si
    mov si, ax
    mov ah, 0x0E

    ler_byte:
        lodsb
        or al, al
        jz fim_print
        int 0x10
        jmp ler_byte
    
    fim_print:
        pop si
        pop ax
        ret
    
primo: db " e primo", 0
nao_primo: db " nao e primo", 0

times 510 - ($ - $$) db 0
dw 0XAA55