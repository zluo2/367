	.text
	.global func5
	.type   func5,@function

func5:

	movq	$0, %r8			#
	movq	$1, %rsi		#
					#
	movq	$0, %rax		#
	cmpq	%rdi, %rsi		#
	jge	func5_end		#
					#
func5_body:				#
	# NOTE: rdx:rax is the 128-bit numerator for idivq instruction.
	cqto				#
	movq	%rdi, %rax		#
	idivq	%rsi			#
					#
	cmpq	$0, %rdx		#
	jnz	func5_check		# 
					#
	addq	%rsi, %r8		# 
func5_check:				#
	incq	%rsi			# 
	cmpq	%rsi, %rdi		# 
	jnz	func5_body		# 
					#
func5_decide:				#
	movq	$0, %rax		# 
					#
	cmpq	%rdi, %r8		# 
	jle	func5_end		# 
					#
	incq	%rax			# 
	#	movq	%r8, %rax	#
func5_end:				#
	ret				#
					#
	.size   func5, .-func5
