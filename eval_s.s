.global eval_s
.global isdigit_s
.global number_s
.global factor_s
.global term_s
.global expression_s

# a0 - char *expr_str
# a1 - int *pos
# t0 - int value
# t1 - char token
# t2 - int pos_val

expression_s:
    addi sp, sp, -64                          
    sd ra, (sp)                              

    sd a0, 8(sp)                              # store a0 on stack
    sd a1, 16(sp)                             # store a1 on stack

    call term_s                               # call term_s function
    mv t0, a0                                 # t0 (value) = term_s(expr_str, pos)

    ld a0, 8(sp)                              # restore a0 from stack
    ld a1, 16(sp)                             # restore a1 from stack

expression_while:
    lw t2, (a1)                               # t2 (pos_val) = *a1 (pos)
    add t3, a0, t2                            # t3 = a0 (*expr_str) + t2 (pos_val)
    lb t1, (t3)                               # t1 (token) = *t3
    li t4, '+'                                # load ASCII value of '+'
    beq t1, t4, expression_while_cont         # if token is '+', jump to expression_while_cont
    li t5, '-'                                # load ASCII value of '-'
    bne t1, t5, expression_while_done         # if token is not '-', jump to expression_while_done

expression_while_cont:
    addi t2, t2, 1                            # increment position value
    sw t2, (a1)                               # update position in memory

    bne t1, t4, expression_while_else         # if token is not '+', jump to expression_while_else

    sd a0, 8(sp)                              # preserve registers before function call
    sd a1, 16(sp)
    sd t0, 24(sp)
    sd t1, 32(sp)
    sd t2, 40(sp)
    sd t4, 48(sp)
    sd t5, 56(sp)

    call term_s                               

    ld t0, 24(sp)                             # restore t0 (value) from stack
    add t0, t0, a0                            # t0 (value) = t0 (value) + a0

    ld a0, 8(sp)                              # restore the rest of the saved registers
    ld a1, 16(sp)
    ld t1, 32(sp)
    ld t2, 40(sp)
    ld t4, 48(sp)
    ld t5, 56(sp)

    j expression_while                        # jump back to start of while loop

expression_while_else:
    bne t1, t5, expression_while_done         # if token is not '-', jump to expression_while_done

    sd a0, 8(sp)                              # preserve registers before function call
    sd a1, 16(sp)
    sd t0, 24(sp)
    sd t1, 32(sp)
    sd t2, 40(sp)
    sd t4, 48(sp)
    sd t5, 56(sp)

    call term_s

    ld t0, 24(sp)                             # restore t0 (value) from stack
    sub t0, t0, a0                            # t0 (value) = t0 (value) - a0

    ld a0, 8(sp)                              # restore the rest of the saved registers
    ld a1, 16(sp)
    ld t1, 32(sp)
    ld t2, 40(sp)
    ld t4, 48(sp)
    ld t5, 56(sp)

    j expression_while                        

expression_while_done:
    mv a0, t0                                 # a0 = t0 (value)
    ld ra, (sp)                               
    addi sp, sp, 64                           
    ret                                       

############################################

# a0 - char *expr_str
# a1 - int *pos
# t0 - int value
# t1 - char token
# t2 - int pos_val

term_s:
    addi sp, sp, -64                         
    sd ra, (sp)                               
    sd a0, 8(sp)                             
    sd a1, 16(sp)           

    call factor_s                             
    mv t0, a0                                 # t0 (value) = result of factor_s(expr_str, pos)

    ld a0, 8(sp)                             
    ld a1, 16(sp)                            

term_while:
    lw t2, (a1)                               # t2 (pos_val) = *a1 (pos)
    add t3, a0, t2                            # t3 = a0 (*expr_str) + t2 (pos_val)
    lb t1, (t3)                               # t1 (token) = *t3
    li t4, '*'                                # load ASCII value of '*'
    beq t1, t4, term_while_cont               # if token is '*', jump to term_while_cont
    li t5, '/'                                # load ASCII value of '/'
    bne t1, t5, term_while_done               # if token is not '/', jump to term_while_done

term_while_cont:
    addi t2, t2, 1                            # increment position value
    sw t2, (a1)                               # update position in memory

    bne t1, t4, term_while_else               # if token is not '*', jump to term_while_else

    sd a0, 8(sp)                              
    sd a1, 16(sp)
    sd t0, 24(sp)
    sd t1, 32(sp)
    sd t2, 40(sp)
    sd t4, 48(sp)
    sd t5, 56(sp)

    call factor_s                            

    ld t0, 24(sp)                             # restore t0 (value) from stack
    mul t0, t0, a0                            # t0 (value) = t0 (value) * result of factor_s

    ld a0, 8(sp)                              
    ld a1, 16(sp)
    ld t1, 32(sp)
    ld t2, 40(sp)
    ld t4, 48(sp)
    ld t5, 56(sp)

    j term_while                              

term_while_else:
    bne t1, t5, term_while_done               # if token is not '/', jump to term_while_done

    sd a0, 8(sp)                              
    sd a1, 16(sp)
    sd t0, 24(sp)
    sd t1, 32(sp)
    sd t2, 40(sp)
    sd t4, 48(sp)
    sd t5, 56(sp)

    call factor_s                            

    ld t0, 24(sp)                             # restore t0 (value) from stack
    div t0, t0, a0                            # t0 (value) = t0 (value) / result of factor_s

    ld a0, 8(sp)                              
    ld a1, 16(sp)
    ld t1, 32(sp)
    ld t2, 40(sp)
    ld t4, 48(sp)
    ld t5, 56(sp)

    j term_while                              

term_while_done:
    mv a0, t0                                 # a0 = t0 (value)
    ld ra, (sp)                               
    addi sp, sp, 64                           
    ret                                       


############################################

# a0 - char *expr_str
# a1 - int *pos
# a2 - int value
# a3 - char token
# t0 - ascii var '(' 40 
# t1 - ascii var ')' 41 
# t6 - temp val reg
# returns value

factor_s:
    addi sp, sp, -64                         
    sd ra, (sp)                               
    sd a0, 8(sp)                              
    sd a1, 16(sp)                            
    sd a2, 24(sp)                           
    sd a3, 32(sp)                            
    sd t0, 40(sp)                          
    sd t1, 48(sp)                          

    li t0, '('                                
    li t1, ')'                                

    lw t2, (a1)                               # load position value
    add t3, a0, t2                            # calculate address of current character
    lb a3, (t3)                               # load current character

    beq a3, t0, factor_s_if                   # if current character is '(', jump to factor_s_if

    call number_s                             

    mv t6, a0                                 # store result in t6

    j factor_done                            

factor_s_if:
    sd a0, 8(sp)                              
    sd a1, 16(sp)
    sd a2, 24(sp)
    sd a3, 32(sp)
    sd t0, 40(sp)
    sd t1, 48(sp)

    addi t2, t2, 1                            # increment position value
    sw t2, (a1)                               # update position in memory

    call expression_s                         

    mv t6, a0                                 # store result in t6

    ld a0, 8(sp)                              
    ld a1, 16(sp)                             
    ld a3, 32(sp)                             

    lw t2, (a1)                               # load position value
    add t3, a0, t2                            # calculate address of current character
    lb a3, (t3)                               # load current character

    bne a3, t1, factor_error                  

    addi t2, t2, 1                            # increment position value
    sw t2, (a1)                               # update position in memory

factor_done:
    mv a0, t6                                 # Return value in a0
    ld ra, (sp)                               
    addi sp, sp, 64                           
    ret                                       

factor_error:
    li a0, 0                                  # return 0 on error
    ret                                       


############################################

# a0 - char c 
# t0 - ascii var '0' 48
# t1 - ascii var '9' 57
# return 0 if true 1 if false 

isdigit_s:
    li t0, '0'                               
    li t1, '9'                               

    blt a0, t0, not_isdigit_s                 # if char is less than '0', jump to not_isdigit_s
    bgt a0, t1, not_isdigit_s                 # if char is greater than '9', jump to not_isdigit_s

    li a0, 1                                  # return 1 if char is a digit
    ret

not_isdigit_s:
    li a0, 0                                  # return 0 if char is not a digit
    ret

############################################

# a0 - char *expr_str
# a1 - int *pos
# a2 - int value
# a3 - char token
# a4 - temp 
# t0 - ascii var '0' 48
# t1 - 10 

number_s:
    addi sp, sp, -48                          
    sd ra, (sp)                               

    li a2, 0                                  
    li t0, 48                                 
    li t1, 10                                 

number_s_while:
    lw t2, (a1)                               # load position value
    add t3, a0, t2                            # calculate address of current character
    lb a3, (t3)                               # load current character

    beqz a3, number_s_while_done              #if char is null, exit loop

    sd a0, 8(sp)                              
    sd a1, 16(sp)
    sd t0, 24(sp)
    sd t1, 32(sp)
    sd t2, 40(sp)

    mv a0, a3                                 # move current character to a0

    call isdigit_s                            

    beqz a0, error                            # if not a digit, jump to error

    ld a0, 8(sp)                             
    ld a1, 16(sp)
    ld t0, 24(sp)
    ld t1, 32(sp)
    ld t2, 40(sp)

    sub a3, a3, t0                            # convert char to integer
    mul a2, a2, t1                            # multiply current value by 10
    add a2, a2, a3                            # add converted integer to value

    addi t2,t2, 1                             # increment position value
    sw t2, (a1)                               # update position in memory

    j number_s_while                          

error:
    li a0, 0                                  # return 0 on error

number_s_while_done:
    mv a0, a2                                 # return value in a0
    ld ra, (sp)                               
    addi sp, sp, 48                           
    ret

############################################

# a0 - char *expr_str 	
# a1 - int pos 

eval_s:
    addi sp, sp, -16                          
    sd ra, (sp)                               

    addi a1, sp, 8                            # point a1 to position on stack
    sw zero, (a1)                             # initialize position to 0

    call expression_s                        

    ld ra, (sp)                              
    addi sp, sp, 16                          
    ret                                       
