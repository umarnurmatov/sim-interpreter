test1:
li x1, 12
addi x2, x1, 30
li x30, 42
beq x2, x30, test2
j fail
test2:
li x1, 10
addi x2, x1, -20
li x30, -10
beq x2, x30, test3
j fail
test3:
li x1, -1
addi x2, x1, 1
li x30, 0
beq x2, x30, test4
j fail
test4:
li x1, 1
addi x2, x1, -32768
li x30, -32767
beq x2, x30, test5
j fail
test5:
li x1, -1
addi x1, x1, 32767
li x30, 32766
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
