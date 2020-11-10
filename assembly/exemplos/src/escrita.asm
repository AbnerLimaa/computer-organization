org 0x7C00
bits 16

main:
    mov ax, 0
    mov ds, ax 
    cli

    mov si, msg
    call prints

    mov si, ola

    call prints
    call prints
    call prints

    hlt

prints:
    push si
    mov ah, 0x0E

    ler_texto:
        lodsb
        or al, al
        jz fim_prints
        int 0x10
        jmp ler_texto

    fim_prints:
        pop si
        ret

msg:    db "Hello world!", 13, 10, 0
ola:    db "Ol", 160, " mundo!", 13, 10

times 510 - ($-$$) db 0
dw 0xAA55