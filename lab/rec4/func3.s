        .text
	.global func3
	        .type   func3,@function
func3:
	movq	$0, %rax		# 
	movq	$0, %rdx		# 
	jmp func3_check			# 
func3_loop:				# 
	addq	(%rdi,%rdx,8), %rax	# 
	incq	%rdx			# 
func3_check:				# 
	cmpq    %rdx, %rsi		# 
	jg      func3_loop		# 
func3_end:				# 
	ret				# 
	
	.size   func3, .-func3

	