org 0x7C00
bits 16

main:
	mov ax, 0
	mov ds, ax
	cli

	mov ax, perg
	call println

	mov ax, 0x7E00
	call gets

	mov ax, resp
	call println

	mov ax, 0x7E00
	call println

	hlt

gets:	
	push ax
	push di
	mov di, ax

	escrever_texto: 
		mov ah, 0 		
		int 0x16
		cmp al, 13
		je finaliza_gets
		mov [ds:di], al
		inc di
		mov ah, 0x0E
		int 0x10
		jmp escrever_texto

	finaliza_gets:	
		mov ah, 0x0E 	
		int 0x10
		mov al, 10
		int 0x10
		mov [ds:di], byte 0 	
		pop di
		pop ax
		ret

println:	
	push ax
	push si
	mov si, ax
	mov ah, 0x0E

	ler_texto:	
		lodsb			
		or al, al
		jz finaliza_println
		int 0x10
		jmp ler_texto	

	finaliza_println:
		mov al, 10
		int 0x10
		mov al, 13
		int 0x10
		pop si
		pop ax
		ret

perg:	db "Oi! Qual o seu nome?", 0
resp:	db "Ola: ", 0

times 510 - ($-$$) db 0
dw 0xAA55