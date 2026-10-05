test1:
li x1, 0
cls x2, x1
li x30, 0
beq x2, x30, test2
j fail
test2:
li x1, -1
cls x2, x1
li x30, 32
beq x2, x30, test3
j fail
test3:
li x1, -32768
cls x2, x1
li x30, 17
beq x2, x30, test4
j fail
test4:
li x1, -2
cls x1, x1
li x30, 31
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
