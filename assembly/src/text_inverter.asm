org 0X7C00
bits 16

mov ax, 0
mov ds, ax
cli

main:
    call text_inverter
    hlt

text_inverter:
    mov cx, 0

    read:
        mov ah, 0
        int 0x16
        cmp al, 13
        je write
        push ax
        inc cx
        mov ah, 0x0E
        int 0x10
        jmp read
    
    write:
            mov ah, 0x0E
            mov al, 13
            int 0x10
            mov al, 10
            int 0x10

            phrase:
                cmp cx, 0
                je end_text_inverter
                dec cx
                pop ax
                mov ah, 0x0E
                int 0x10
                jmp phrase

    end_text_inverter:
        ret

times 510 - ($ - $$) db 0
dw 0xAA55