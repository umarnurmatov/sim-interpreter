test1:
li x1, 12
li x2, 30
add x3, x1, x2
li x30, 42
beq x3, x30, test2
j fail
test2:
li x1, -12
li x2, -30
add x3, x1, x2
li x30, -42
beq x3, x30, test3
j fail
test3:
li x1, -1
li x2, 1
add x3, x1, x2
li x30, 0
beq x3, x30, test4
j fail
test4:
li x1, 17
li x2, -5
add x2, x1, x2
li x30, 12
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
