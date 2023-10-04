.global get_bitseq_signed_s
.global get_bitseq_s

/* get_bitseq_signed_s calculates the value of a sequence of bits as a signed int
    a0 is the number
    a1 is the start index
    a2 is the end index
*/

get_bitseq_signed_s:
    addi sp, sp, -8         # allocate stack space
    sd ra, (sp)             # *sp = ra

    jal get_bitseq_s        # get the sequence as unsigned
    
    sub t0, a2, a1          # end - start
    addi t0, t0, 1          # length = end - start + 1

    li t1, 64               # registers are 64 bits wide
    sub t1, t1, t0          # t1 is the sign extension amount

    sllw t2, a0, t1          # shift desired bit seq to the top/left
    sra a0, t2, t1          # shift right/down arithmetic to sign extend

    ld ra, (sp)             # ra = *sp
    addi sp, sp, 8          # deallocate stack space
    ret
