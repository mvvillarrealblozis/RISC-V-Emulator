.global swap_s

/* swap two elements of an integer array
    a0 - int arr[]
    a1 - int i
    a2 - int j
*/

swap_s:
    li t0, 4
    mul t1, a1, t0
    add t1, a0, t1
    mul t2, a2, t0
    add t2, a0, t2
    lw t3, (t1)   
    lw t4, (t2)
    sw t4, (t1)
    sw t3, (t2)
    ret
