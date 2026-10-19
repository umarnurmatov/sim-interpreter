addi x0, x0, 0
addi x0, x0, 0
li x0, 0
addi x0, x0, 0
addi x0, x0, 0
li x1, 1024
addi x0, x0, 0
addi x0, x0, 0
li x5, 8192
outer:
addi x0, x0, 0
addi x0, x0, 0
li x2, 8192
addi x0, x0, 0
addi x0, x0, 0
li x3, 0
addi x0, x0, 0
addi x0, x0, 0
li x4, 0
loop:
addi x0, x0, 0
addi x0, x0, 0
beq x3, x0, even
addi x0, x0, 0
addi x0, x0, 0
nor x3, x3, x3
addi x0, x0, 0
addi x0, x0, 0
j hop1
even:
addi x0, x0, 0
addi x0, x0, 0
nor x3, x3, x3
addi x0, x0, 0
addi x0, x0, 0
j hop3
hop2:
addi x0, x0, 0
addi x0, x0, 0
beq x3, x0, hop4
addi x0, x0, 0
addi x0, x0, 0
j hop4
hop1:
addi x0, x0, 0
addi x0, x0, 0
j hop3
hop4:
addi x0, x0, 0
addi x0, x0, 0
addi x4, x4, 1
addi x0, x0, 0
addi x0, x0, 0
addi x2, x2, -1
addi x0, x0, 0
addi x0, x0, 0
beq x2, x0, check
addi x0, x0, 0
addi x0, x0, 0
j loop
hop3:
addi x0, x0, 0
addi x0, x0, 0
beq x3, x0, hop2
addi x0, x0, 0
addi x0, x0, 0
j hop2
check:
addi x0, x0, 0
addi x0, x0, 0
beq x4, x5, check_phase
addi x0, x0, 0
addi x0, x0, 0
j fail
check_phase:
addi x0, x0, 0
addi x0, x0, 0
beq x3, x0, next
addi x0, x0, 0
addi x0, x0, 0
j fail
next:
addi x0, x0, 0
addi x0, x0, 0
addi x1, x1, -1
addi x0, x0, 0
addi x0, x0, 0
beq x1, x0, success
addi x0, x0, 0
addi x0, x0, 0
j outer
success:
addi x0, x0, 0
addi x0, x0, 0
li x0, 0
addi x0, x0, 0
addi x0, x0, 0
li x8, 93
addi x0, x0, 0
addi x0, x0, 0
syscall
fail:
addi x0, x0, 0
addi x0, x0, 0
li x0, 1
addi x0, x0, 0
addi x0, x0, 0
li x8, 93
addi x0, x0, 0
addi x0, x0, 0
syscall
