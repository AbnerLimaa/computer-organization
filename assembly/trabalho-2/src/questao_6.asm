org 0x7C00
bits 16

mov ax, 0
mov ds, ax
cli

main:   
    call fibonacci
    
    hlt

fibonacci:
    push ax
    push bx
    push cx
    push dx

    mov ax, 1
    mov bx, 0
    mov cx, 22

    calc_num:
        cmp cx, 0
        je fim_fibonacci
        call print_num
        mov dx, ax
        add ax, bx
        mov bx, dx
        dec cx
        jmp calc_num

    fim_fibonacci:
        pop dx
        pop cx
        pop bx
        pop ax
        ret   

print_num: 
	push ax
    push bx
    push cx
	mov bx, 10
	mov cx, 0

	ler_algarismos:	
		mov dx, 0    
		idiv bx
		add dx, 48   
		push dx
		inc cx
		or ax, ax
		jnz ler_algarismos

	imprime_algarismos:	
		pop ax        
		mov ah, 0x0E
		int 0x10
		dec cx
		or cx, cx
		jnz imprime_algarismos
        jmp fim_print_num
		
    fim_print_num:
        mov al, 32
        int 0x10
        pop cx
        pop bx
	    pop ax
	    ret

times 510 - ($ - $$) db 0
dw 0xAA55