test1:
li x0, 0
li x1, 10
li x2, 0
li x3, 1
loop:
beq x1, x0, check
add x4, x2, x3
addi x2, x3, 0
addi x3, x4, 0
addi x1, x1, -1
j loop
check:
li x30, 55
beq x2, x30, success
j fail
success:
li x0, 0
li x8, 93
syscall
fail:
li x0, 1
li x8, 93
syscall
