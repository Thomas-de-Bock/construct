# About
Construct is an abstraction over x86 NASM Assembly. It adds features such as while loops, if statements, scoped macros, simpler function-call syntax and more.
It currently supports 64, 32, 16 and 8 bit modes.

# Syntax
Construct is indent sentitive and requires all indentations to use the tab character. Below is a code example:
```
extern printf

section .text
function strlwr(str):
	while byte[str] ne 0:
		if byte[str] ge 65:
			if byte[str] le 90:
				!crntchr sil
				mov crntchr, byte[str]
				add crntchr, 32
				mov byte[str], crntchr
		inc str


function main():
	call strlwr(teststring)

	mov rax, 0
	call printf(fmt, teststring)

	mov rax, 60
	syscall

section .data
teststring db "HeLlO WoRlD", 0
fmt: db "%s", 10, 0
```
- Sections: Sections do not add any indentation, construct currently supports text, data and bss sections.
- While loops: While loops take a single [conditional](#conditionals) statement
- If statements: If statements, like while loops, take a single [conditional](#conditionals) statement
- Functions:
  Functions are declared with the "function" keyword, a "ret" instruction is added to functions in post-processing, so functions will not flow into eachother.
- Function calls: Functions can be called with any number of arguments, independent of the function decleration. 
  If the amount of arguments used to call a function is more than its decleration states, they can be accessed like normal with their respective registers / stack address.
  Construct function calls, like NASM, use the "call" keyword. Functions can still be called without parentheses or arguments, NASM-style.
- Macros: Construct macros can only be used in their respective scopes. Construct macros are declared with the '!' character and cannot contain whitespaces.

Any NASM code can still be used in your construct programs.

## Conditionals
Using the naming of the conditional jump instructions, construct supports the following comparisons:
- `e: equals`
- `ne: not equals`
- `l: less`
- `g: greater`
- `le: less or equal`
- `ge: greater or equal`

Neither side of the comparison can contains whitespaces.

# Use
### Required flags
- `-f (format)`: Can be either "elf64", "elf32", "elf16", "elf8" and decides the registers used for funcion calls.
- `-i (input file)`: Specifies the input file to be compiled (-i is not neccesary)
