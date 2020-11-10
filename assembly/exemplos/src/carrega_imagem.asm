org 0x7C00  
bits 16     

mov ax, 0
mov ds, ax
cli

main:	
	mov ah, 0
	mov al, 0x13 
	int 0x10

	int 0x13
	mov ah, 0x02 
	mov al, 18  
	mov cl, 2  
	mov ch, 0  
	mov dh, 0  
	mov bx, 0x7E00 
	int 0x13

	call carrega_paleta	

	mov ax, 0xA000
	mov es, ax
	mov di, 0
	mov si, 0x8100

	jmp movimenta_imagem

carrega_paleta:
	mov si, 0x7E00

	mov ax, 0
	mov dx, 0x3C8
	out dx, ax
	
	mov dx, 0x3C9

	.mudar_cor_registrador:	
		push ax
		mov al, [ds:si]
		out dx, ax
		inc si
	
		mov al, [ds:si]
		out dx, ax
		inc si

		mov al, [ds:si]
		out dx, ax
		inc si

		pop ax

		inc ax
		cmp ax, 256
		jne .mudar_cor_registrador
	
	ret

print_imagem: 
	push di
	push si
	mov cx, 0

	.print_pixel:	
		mov al, [ds:si]
		cmp al, 0
		je .pular_pixel
		mov [es:di], al

	.pular_pixel:	
		inc si
		inc di
		inc cx
		mov dx, 0
		mov ax, cx
		mov bx, 81
		idiv bx
		cmp dx, 0
		jne .print_pixel
		add di, 239
		cmp cx, 8100
		jne .print_pixel

	pop si
	pop di
	ret

movimenta_imagem:
	call print_imagem

	mov ah, 0
	int 0x16

	cmp al, 'd'
	je .direita

	cmp al, 'a'
	je .esquerda

	cmp al, 's'
	je .baixo

	cmp al, 'w'
	je .cima

	jmp movimenta_imagem
		
	.direita:
		add di, 1
		jmp movimenta_imagem

	.esquerda:
		sub di, 1
		jmp movimenta_imagem

	.baixo:
		add di, 320
		jmp movimenta_imagem

	.cima:
		sub di, 320
		jmp movimenta_imagem
	
times 510 - ($-$$) db 0
dw 0xAA55