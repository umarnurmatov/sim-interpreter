test1:
j test2
j fail
test2:
li x1, 42
j test3
j fail
test3:
li x30, 42
beq x1, x30, test4
j fail
test4:
j success
j fail
success:
li x0, 0
li x8, 93
syscall
fail:
li x0, 1
li x8, 93
syscall
