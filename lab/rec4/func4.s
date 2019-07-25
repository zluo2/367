        .text
	.global func4
	        .type   func4,@function
func4:
	cmpq	$0, %rdi	# 
	sete	%r8b		# 
	cmpq	$0,%rsi		# 
	sete	%r9b		# 
	orb	%r8b, %r9b	# 
	cmpb	$1, %r9b	# 
	jne	func4_L0	# 
	movq	$0, %rax	# 
	ret			# 
func4_L0:			# 
	cmpq	$0, %rdi	# 
	jg	func4_L3	# 
func4_L1:			# 
	cmpq	$0, %rsi	# 
	jg	func4_L2	# 
	movq	$3, %rax	# 
	jmp func4_end		# 
func4_L2:			# 
	movq	$2, %rax	# 
	jmp func4_end		# 
				# 
func4_L3:			# 
	cmpq	$0, %rsi	# 
	jg	func4_L4	# 
	movq	$4, %rax	# 
	jmp func4_end		# 
func4_L4:			# 
	movq	$1, %rax	# 
func4_end:			# 
	ret			# 
	
	.size   func4, .-func4

