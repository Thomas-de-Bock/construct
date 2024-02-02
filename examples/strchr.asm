global _start
extern printf
section .text
strchr:
mov rax, 0
startwhile0:
cmp byte[rdi], 0
je endwhile0
cmp byte[rdi], sil
jne endif0
mov rax, rdi
ret
endif0:
inc rdi
jmp startwhile0
endwhile0:
ret
_start:
mov rdi, teststr
mov rsi, 87
call strchr
mov rdi, fmt
mov rsi, rax
call printf
mov rdi, fmt
mov rsi, teststr
call printf
mov rax, 60
syscall
ret
section .data
teststr: db "Hello World!", 0
fmt: db "%p", 10, 0
