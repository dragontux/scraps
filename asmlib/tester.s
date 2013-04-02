%include "stdio.inc"
global _start

section .data
	msg db "file to read > ",0
	file db "/etc/passwd",0
	badmsg db "Bad file name.",0xa,0

section .bss
	asdf resb 128
	fd resw 1

section .text

_start:
	mov rdi, msg
	call puts

	mov rdi, asdf
	call gets

	;mov rdi, asdf
	;.loop:
	;	call gets
	;	call puts
	;jmp .loop

	mov rdi, asdf
	call open
	mov rbx, rax

	cmp rbx, 0
	jl .bad

.loop2:
	mov rdi, rbx
	call getc

	cmp rax, -1
	je .end

	mov rdi, rax
	call putchar

	jmp .loop2

.bad:
	mov rdi, badmsg
	call puts

	.end:

	mov rdi, 0
	call exit
