org 0x7C00
bits 16

mov ax, 0
mov ds, ax 
cli

main:
    int 0x13
    mov ah, 0x02       
    mov al, 1          
    mov ch, 0          
    mov dh, 0          
    mov cl, 2          
    mov bx, 0x7E00     
    int 0x13           

    mov si, bx    
    mov bx, 0xB800
    mov es, bx

    mov ax, 0
    mov bx, 0

    string:
        lodsb
        mov di, bx
        mov [es:di], al
        add bx, 2
        or ax, ax
        jnz string
        
    hlt

times 510 - ($-$$) db 0
dw 0xAA55