test1:
li x1, 11
li x2, 85
bdep x3, x1, x2
li x30, 69
beq x3, x30, test2
j fail
test2:
li x1, 11
li x2, 0
bdep x3, x1, x2
li x30, 0
beq x3, x30, test3
j fail
test3:
li x1, 0
li x2, 85
bdep x3, x1, x2
li x30, 0
beq x3, x30, test4
j fail
test4:
li x1, -32768
li x2, -1
bdep x3, x1, x2
li x30, -32768
beq x3, x30, test5
j fail
test5:
li x1, 11
li x2, 85
bdep x2, x1, x2
li x30, 69
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
