li x1, 10
li x2, 0
li x3, 1
beq x1, x0, 6
add x4, x2, x3
addi x2, x3, 0
addi x3, x4, 0
addi x1, x1, -1
j 3
