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
    call decrypt

    mov si, 0x8000
    call print

    hlt

decrypt:
    push ax
    push bx
    push di

    mov bx, 0x7E00
    mov si, code 

    read_byte: 
        lodsb
        cmp ax, 10
        je reassign_si
        cmp bx, 0x8000
        je end_decrypt
        mov dx, [bx]
        sub dx, ax
        mov [di], dx
        inc di
        inc bx
        jmp read_byte
    
    reassign_si:
        mov si, code
        jmp read_byte

    end_decrypt:
        pop di
        pop bx
        pop ax
        ret

print:
    push si
    mov ah, 0x0E

    read_text:
        lodsb
        or al, al
        jz end_print
        int 0x10
        jmp read_text

    end_print:
        pop si
        ret

code: db 3, 9, 8, 0, 6, 7, 10
           
times 510 - ($ - $$) db 0
dw 0xAA55