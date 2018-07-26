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

    calculate_number:
        cmp cx, 0
        je end_fibonacci
        call print_number
        mov dx, ax
        add ax, bx
        mov bx, dx
        dec cx
        jmp calculate_number

    end_fibonacci:
        pop dx
        pop cx
        pop bx
        pop ax
        ret   

print_number: 
	push ax
    push bx
    push cx
	mov bx, 10
	mov cx, 0

	read_algarisms:	
		mov dx, 0    
		idiv bx
		add dx, 48   
		push dx
		inc cx
		or ax, ax
		jnz read_algarisms

	print_algarisms:	
		pop ax        
		mov ah, 0x0E
		int 0x10
		dec cx
		or cx, cx
		jnz print_algarisms
        jmp end_print_number
		
    end_print_number:
        mov al, 32
        int 0x10
        pop cx
        pop bx
	    pop ax
	    ret

times 510 - ($ - $$) db 0
dw 0xAA55