test1:
li x0, 0
li x8, 93
syscall
j fail
success:
li x0, 0
li x8, 93
syscall
fail:
li x0, 1
li x8, 93
syscall
