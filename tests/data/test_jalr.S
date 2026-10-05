test1:
li x1, 28
jalr x2, x1, 0
j fail
j fail
j fail
j fail
j fail
target1:
li x30, 8
beq x2, x30, test2
j fail
test2:
li x1, 57
jalr x3, x1, -5
j fail
target2:
li x30, 48
beq x3, x30, success
j fail
success:
li x0, 0
li x8, 93
syscall
fail:
li x0, 1
li x8, 93
syscall
