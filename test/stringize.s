	.file	"stringize.c"
	.section	.rodata
.LC0:
	.string	"HelloWorld"
.LC1:
	.string	"CAT(\"Hello\", \"World\")= %d"
.LC2:
	.string	"\n%d\n"
	.text
.globl main
	.type	main, @function
main:
	leal	4(%esp), %ecx
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ecx
	subl	$36, %esp
	movl	$5, -8(%ebp)
	movl	$.LC0, 4(%esp)
	movl	$.LC1, (%esp)
	call	printf
	movl	-8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC2, (%esp)
	call	printf
	movl	$0, %eax
	addl	$36, %esp
	popl	%ecx
	popl	%ebp
	leal	-4(%ecx), %esp
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 4.1.0 20060304 (Red Hat 4.1.0-3)"
	.section	.note.GNU-stack,"",@progbits
