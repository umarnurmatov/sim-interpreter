test1:
li x1, 123
li x30, 123
beq x1, x30, test2
j fail
test2:
li x2, -1
li x30, -1
beq x2, x30, test3
j fail
test3:
li x3, -32768
li x30, -32768
beq x3, x30, test4
j fail
test4:
li x31, 32767
li x30, 32767
beq x31, x30, success
j fail
success:
li x0, 0
li x8, 93
syscall
fail:
li x0, 1
li x8, 93
syscall
