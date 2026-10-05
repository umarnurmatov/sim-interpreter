test1:
li x1, 7
li x2, 7
beq x1, x2, test2
j fail
test2:
li x1, 7
li x2, 8
beq x1, x2, fail
j test3
test3:
li x1, -1
li x2, -1
beq x1, x2, test4
j fail
test4:
beq x1, x1, test5
j fail
test5:
li x30, 1
beq x1, x30, fail
j success
success:
li x0, 0
li x8, 93
syscall
fail:
li x0, 1
li x8, 93
syscall
