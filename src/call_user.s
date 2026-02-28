global call_user

call_user:
	mov ax, 0x28
	ltr ax
	
	mov ax, 0x23
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	push 0x23
	push edx ; second arg of call_user stack
	pushf
	push 0x1B
	push ecx ; first arg of call_user fct
	iret

