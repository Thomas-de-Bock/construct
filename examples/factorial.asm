global _start
extern printf
section .text
factorial:
mov rsi, 2
mov rax, 1
startwhile0:
cmp rsi, rdi
jg endwhile0
mul rsi
inc rsi
jmp startwhile0
endwhile0:
ret
_start:
mov rdi, 3
call factorial
mov rdi, fmt
mov rsi, rax
call printf
mov rax, 60
syscall
ret
section .data
fmt: db "%d", 10, 0
