test1:
li x1, 512
li x2, 123
st x2, [x1, 0]
ld x3, [x1, 0]
li x30, 123
beq x3, x30, test2
j fail
test2:
li x1, 508
li x2, -1
st x2, [x1, 4]
ld x3, [x0, 512]
li x30, -1
beq x3, x30, test3
j fail
test3:
li x1, 516
li x2, -32768
st x2, [x1, -4]
ld x3, [x0, 512]
li x30, -32768
beq x3, x30, test4
j fail
test4:
li x1, 1020
li x2, 32767
st x2, [x1, 0]
ld x3, [x1, 0]
li x30, 32767
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
