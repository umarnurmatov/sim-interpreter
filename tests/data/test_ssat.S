test1:
li x1, 42
ssat x2, x1, 8
li x30, 42
beq x2, x30, test2
j fail
test2:
li x1, 200
ssat x2, x1, 8
li x30, 127
beq x2, x30, test3
j fail
test3:
li x1, -200
ssat x2, x1, 8
li x30, -128
beq x2, x30, test4
j fail
test4:
li x1, 1
ssat x2, x1, 1
li x30, 0
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
