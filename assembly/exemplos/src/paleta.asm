org 0x7C00
bits 16

mov ax, 0
mov ds, ax 
cli

main:
	mov ax, msg1	
	call print
	mov ax, 0x7E00
	call getnum

	mov ax, msg2	
	call print
	mov ax, 0x7E02
	call getnum

	mov ax, msg3	
	call print
	mov ax, 0x7E04
	call getnum

	mov al, 0
    mov dx, 0x3C8
    out dx, al       

    mov dx, 0x3C9

	mov ax, [0x7E00]
    out dx, al       

    mov ax, [0x7E02]
    out dx, al       

	mov ax, [0x7E04]
    out dx, al       

	jmp main

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

print:	
	push ax
	push si
	mov si, ax
	mov ah, 0x0E

	ler_texto:	
		lodsb			
		or al, al
		jz fim_print
		int 0x10
		jmp ler_texto	
		
	fim_print:	
		pop si
		pop ax
		ret

fim:
    hlt

msg1:	db "Valor Vermelho: ", 0
msg2:	db "Valor Verde: ", 0
msg3:	db "Valor Azul: ", 0

times 510 - ($-$$) db 0
dw 0xAA55