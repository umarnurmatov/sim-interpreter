li x0, 0
li x1, 4096
li x2, 10000
li x3, 20000
add x3, x3, x3
add x3, x1, x3
add x4, x1, x0
li x5, 0
generate:
st x5, [x4, 0]
addi x4, x4, 4
addi x5, x5, 7919
addi x6, x5, -10000
cbit x7, x6, 31
beq x6, x7, wrap
j generated
wrap:
add x5, x6, x0
generated:
beq x4, x3, sort_start
j generate
sort_start:
addi x4, x1, 4
outer:
ld x5, [x4, 0]
add x6, x4, x0
inner:
beq x6, x1, insert
ld x7, [x6, -4]
nor x9, x7, x7
addi x9, x9, 1
add x9, x5, x9
cbit x10, x9, 31
beq x9, x10, insert
st x7, [x6, 0]
addi x6, x6, -4
j inner
insert:
st x5, [x6, 0]
addi x4, x4, 4
beq x4, x3, verify_start
j outer
verify_start:
add x4, x1, x0
li x5, 0
verify:
ld x6, [x4, 0]
beq x6, x5, verified
j fail
verified:
addi x4, x4, 4
addi x5, x5, 1
beq x5, x2, success
j verify
success:
li x0, 0
li x8, 93
syscall
fail:
li x0, 1
li x8, 93
syscall
