	.text
	.global func1
	.type   func1,@function
func1:
	# accepts four parameters:  func1(a,b,c,d)
	movq	%rdi, %rax	
	addq	%rsi, %rax
	addq	%rdx, %rcx
	subq	%rcx, %rax
	ret

	.size   func1, .-func1
