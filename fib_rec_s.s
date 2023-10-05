.global fib_rec_s


# fibrec - compute the nth fibonacci number

# a0 - int n

fib_rec_s:
	addi sp, sp, -32
	sd ra, 24(sp)
	sd a0, 16(sp)				# save value of n to the stack

	li t0, 2					# load 2 in t2
	blt a0, t0, base_cases		# if n is less than 2, go to the base cases

	addi a0, a0, -1				# decrease n by 1 			
	call fib_rec_s				# call fibrec with n - 1
	sd a0, 8(sp)				# store result of fibrec(n - 1) on stack
	
	ld a0, 16(sp)				# get original n from stack
	addi a0, a0, -2				# decrease n by 2 
	call fib_rec_s				# call fibrec with n - 2
	sd a0, 0(sp)				# store result of fibrec(n - 2) on stack

	ld t1, 8(sp)
	ld t2, 0(sp)
	
	add a0, t1, t2				# add results of fibrec(n - 1) and fibrec (n - 2)
	j done						# jump to done

base_cases:
	beqz a0, is_zero			# if n is zero go to is_zero
	li a0, 1					# else n = 1
	j done 						# j done 


is_zero:
	li a0, 0					# n = 0 
	
done:
	ld ra, 24(sp)
	addi sp, sp, 32
	ret	

