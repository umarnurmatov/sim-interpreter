test1:
li x1, -1
cbit x2, x1, 0
li x30, -2
beq x2, x30, test2
j fail
test2:
li x1, 0
cbit x2, x1, 12
li x30, 0
beq x2, x30, test3
j fail
test3:
li x1, 85
cbit x2, x1, 1
li x30, 85
beq x2, x30, test4
j fail
test4:
li x1, 85
cbit x1, x1, 6
li x30, 21
beq x1, x30, success
j fail
success:
li x0, 0
li x8, 93
syscall
fail:
li x0, 1
li x8, 93
syscall
