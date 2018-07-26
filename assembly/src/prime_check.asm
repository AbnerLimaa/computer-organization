org 0x7C00
bits 16

mov ax, 0
mov ds, ax
cli

main:
    mov ax, 0x7E00
    call get_number

    mov ax, 0x7E00
    call prime_test

    hlt

get_number:
    push ax
    push bx
    push dx
    push di
    mov di, ax
    mov dx, 10
    mov bx, 1
    
    read_char:
        mov ah, 0x0
        int 0x16
        cmp al, 13
        je end_get_number
        mov ah, 0x0E
        int 0x10
        mov ah, 0
        sub ax, 48
        imul ax, bx
        imul bx, dx
        add [di], al
        jmp read_char
    
    end_get_number:
        pop di
        pop dx
        pop bx
        pop ax
        ret

prime_test:
    push ax
    push cx
    push si

    mov si, ax
    mov cx, 2

    div_number:
        mov ax, [si]        
        cmp cx, ax
        je end_prime
        div cl
        cmp ah, 0 
        je end_not_prime
        inc cx
        jmp div_number

    end_prime:
        mov ax, prime
        call print
        jmp end_prime_test

    end_not_prime:
        mov ax, not_prime
        call print
        jmp end_prime_test

    end_prime_test:
        pop si
        pop cx
        pop ax
        ret
        
print:
    push ax
    push si
    mov si, ax
    mov ah, 0x0E

    read_byte:
        lodsb
        or al, al
        jz end_print
        int 0x10
        jmp read_byte
    
    end_print:
        pop si
        pop ax
        ret
    
prime: db " is prime", 0
not_prime: db " isnt prime", 0

times 510 - ($ - $$) db 0
dw 0XAA55