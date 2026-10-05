test1:
li x1, 0
li x2, 0
nor x3, x1, x2
li x30, -1
beq x3, x30, test2
j fail
test2:
li x1, 0
li x2, -1
nor x3, x1, x2
li x30, 0
beq x3, x30, test3
j fail
test3:
li x1, 85
li x2, 170
nor x3, x1, x2
li x30, -256
beq x3, x30, test4
j fail
test4:
li x1, -32768
li x2, 0
nor x3, x1, x2
li x30, 32767
beq x3, x30, test5
j fail
test5:
li x1, 5
li x2, 10
nor x1, x1, x2
li x30, -16
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
