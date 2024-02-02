global _start
extern printf
section .text
strlwr:
startwhile0:
cmp byte[rdi], 0
je endwhile0
cmp byte[rdi], 65
jl endif1
cmp byte[rdi], 90
jg endif0
mov sil, byte[rdi]
add sil, 32
mov byte[rdi], sil
endif0:
endif1:
inc rdi
jmp startwhile0
endwhile0:
ret
_start:
mov rdi, teststr
call strlwr
mov rdi, fmt
mov rsi, teststr
call printf
mov rax, 60
syscall
ret
section .data
teststr: db "HeLlO WoRlD", 0
fmt: db "%s", 10, 0
