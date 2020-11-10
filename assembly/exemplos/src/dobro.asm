org 0x7C00
bits 16

main:
	mov ax, 0
	mov ds, ax

	mov ax, 0x7E00
	call getnum

	mov ax, [0x7E00]
	imul ax, 2
	mov [0x7E00], ax

	mov ax, 0x7E00
	call printnum
	
	hlt

getnum:   
	push ax
	mov di, ax

	mov ax, 0
	push ax

	ler_num:	
		mov ah, 0x00  
		int 0x16
		mov ah, 0x0E
		int 0x10      
		cmp al, 13
		je fim_getnum
		sub al, 48    
		mov dl, al
		pop ax
		imul ax, 10
		add ax, dx
		push ax
		jmp ler_num

	fim_getnum:  
		mov al, 10
		int 0x10
        pop ax
		mov [ds:di], ax
		pop ax
		ret

printnum: 
	push ax
	mov si, ax
	mov ax, [ds:si]
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
		
	pop ax
	ret

times 510 - ($-$$) db 0
dw 0xAA55