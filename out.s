	global	main
	extern	printf
	section	.text
LC0:	db	"%d",10,0
printint:
	push	rbp
	mov	rbp, rsp
	sub	rsp, 16
	mov	[rbp-4], edi
	mov	eax, [rbp-4]
	mov	esi, eax
	lea	rdi, [rel LC0]
	mov	eax, 0
	call	printf
	nop
	leave
	ret

	common	i 8:8
	section	.text
	global	main
main:
	push	rbp
	mov	rbp, rsp
	mov	r8, 1
	mov	[i], r8
L1:
	mov	r8, [i]
	mov	r9, 10
	cmp	r8, r9
	jg	L2
	mov	r8, [i]
	mov	rdi, r8
	call	printint
	mov	r8, [i]
	mov	r9, 1
	add	r9, r8
	mov	[i], r9
	jmp	L1
L2:
	mov	eax, 0
	pop	rbp
	ret
