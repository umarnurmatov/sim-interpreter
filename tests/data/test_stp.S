test1:
li x1, 11
li x2, 29
li x3, 512
stp x1, x2, 0(x3)
ld x4, [x0, 512]
ld x5, [x0, 516]
li x30, 11
beq x4, x30, test1b
j fail
test1b:
li x30, 29
beq x5, x30, test2
j fail
test2:
li x1, -1
li x2, 2
li x3, 508
stp x1, x2, 4(x3)
ld x4, [x0, 512]
ld x5, [x0, 516]
li x30, -1
beq x4, x30, test2b
j fail
test2b:
li x30, 2
beq x5, x30, test3
j fail
test3:
li x1, 7
li x2, 13
li x3, 1016
stp x1, x2, 0(x3)
ld x4, [x0, 1016]
ld x5, [x0, 1020]
li x30, 7
beq x4, x30, test3b
j fail
test3b:
li x30, 13
beq x5, x30, success
j fail
success:
li x0, 0
li x8, 93
syscall
fail:
li x0, 1
li x8, 93
syscall
