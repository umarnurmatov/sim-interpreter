li x0, 0
li x1, 1024
li x5, 8192
outer:
li x2, 8192
li x3, 0
li x4, 0
loop:
beq x3, x0, even
nor x3, x3, x3
j hop1
even:
nor x3, x3, x3
j hop3
hop2:
beq x3, x0, hop4
j hop4
hop1:
j hop3
hop4:
addi x4, x4, 1
addi x2, x2, -1
beq x2, x0, check
j loop
hop3:
beq x3, x0, hop2
j hop2
check:
beq x4, x5, check_phase
j fail
check_phase:
beq x3, x0, next
j fail
next:
addi x1, x1, -1
beq x1, x0, success
j outer
success:
li x0, 0
li x8, 93
syscall
fail:
li x0, 1
li x8, 93
syscall
