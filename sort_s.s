.global swap_s
.global sort_s

/* sort_s sorts an array of 32-bit integers in-place,
   in asceding order

   a0 - int arr[]
   a1 - int len

   t0 - int i;
   t1 - int j;

*/

.align 4
sort_s:                     # prologue
    addi sp, sp, -40        # allocate stack space
    sd ra, (sp)             # save return addr
    li t0, 1

floop:
    bge t0, a1, fdone       # reached arr len?
    mv t1, t0               # j = i

wloop:
    ble t1, zero, wdone     # t1 (j) <= 0 ?
    li t3, 4                # t3 = 4;
    mul t4, t1, t3          # t4 = i * 4
    add t5, a0, t4          # t5 = a0 + t4
    addi t6, t5, -4         # t6 = arr[j-1]
    lw t5, (t5)             # t5 = arr[j]
    lw t6, (t6)             # t6 = arr[j-1]
    ble t6, t5, wdone       # arr[j-1] <= arr[j] ?

    sd a0, 8(sp)            # Preserve caller saved regs
    sd a1, 16(sp)
    sd t0, 24(sp)
    sd t1, 32(sp)

    mv a1, t1               # a1 = t1 (j)
    addi a2, t1, -1         # a2 = t1 (j) - 1
    call swap_s

    ld a0, 8(sp)            # Restore caller saved regs
    ld a1, 16(sp)
    ld t0, 24(sp)
    ld t1, 32(sp)

    addi t1, t1, -1         # t1 (j) = t1 (j) - 1
    j wloop

wdone:
    addi t0, t0, 1
    j floop

fdone:                      # epilogue
    ld ra, (sp)             # restore return addr
    add sp, sp, 40          # deallocate
    ret
