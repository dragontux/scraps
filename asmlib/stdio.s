global _start

global putc 	; int putc( int char, int fd );
global putchar 	; int putchar( int char );
global puts	; void puts( char *str );

global getc 	; int getc( int fd );
global getchar 	; int getchar( void );
global gets	; void gets( char *buf );

global open	; int open( char *path, int flags );
global close	; int close( int fd );

global exit 	; int exit( int status );

%define STDIN  0
%define STDOUT 1
%define STDERR 2

%define SYS_READ  	0
%define SYS_WRITE 	1
%define SYS_OPEN  	2
%define SYS_CLOSE	3
%define SYS_EXIT  	60

putc:
	push rsi
	push rdx
	push rdi

	mov rax, SYS_WRITE
	mov rdi, rsi
	mov rsi, rsp
	mov rdx, 1

	syscall

	mov rax, 0

	pop rdi
	pop rdx
	pop rsi
	ret

getc:
	push rsi
	push rdx
	push rdi

	mov rax, SYS_READ
	mov rsi, rsp
	mov rdx, 1

	syscall

	cmp rax, 0
	je .bad

	mov rax, [rsp]
	jmp .end

.bad:
	mov rax, -1

.end:
	pop rdi
	pop rdx
	pop rsi
	ret

putchar:
	push rsi

	mov rsi, STDOUT
	call putc

	pop rsi
	ret

getchar:
	push rsi

	mov rdi, STDIN
	call getc

	pop rdi
	ret

puts:
	push rdi
	push rsi
	push rdx

	mov rsi, rdi

	.loop:
		mov dl, [rsi]
		cmp dl, 0
		je .end

		mov rdi, rdx
		call putchar

		add rsi, 1
		jmp .loop

	.end:
		pop rdx
		pop rsi
		pop rdi

	ret

gets:
	push rdi
	push rsi
	push rdx

	mov rsi, rdi

	.loop:
		call getchar
		cmp al, 0xa
		je .end

		cmp al, 0
		je .end

		mov [rsi], al
		add rsi, 1

		jmp .loop

	.end:
		mov [rsi], byte 0
		pop rdx
		pop rsi
		pop rdi

	ret

exit:
	mov rax, 60
	syscall

open:
	mov rax, SYS_OPEN
	syscall
	ret

close:
	mov rax, SYS_CLOSE
	syscall
	ret
