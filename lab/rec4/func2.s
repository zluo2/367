	.text
	.global func2
	.type   func2,@function
func2:

	movq $0, %rsi		# 
	movq %rdi, %rax         # 
	jmp func2test		# 
func2loop:			#
	# divide by 2.
	# --> divisor in rdx:rax.
	# --> numerator in argument.
	cqto			# sign-extends rax to rdx:rax.
	movq	$2, %r8		# 
	divq 	%r8		#
	
	# --> quotient is in rax, remainder is in rdx.

	incq %rsi		# 
func2test:
	cmpq $1, %rax		# 
	jg func2loop		# 
	movq %rsi, %rax		# 
	ret			#

	.size   func2, .-func2

	