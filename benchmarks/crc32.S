li x0, 0
li x1, 32767
li x10, 1217
li x11, 887
li x12, -256
li x13, 16
constants:
add x10, x10, x10
add x11, x11, x11
add x12, x12, x12
addi x13, x13, -1
beq x13, x0, constants_done
j constants
constants_done:
addi x10, x10, 7607
addi x11, x11, -6425
li x14, 58
message:
li x2, -1
li x3, 49
byte:
bdep x4, x3, x12
nor x5, x2, x4
nor x6, x2, x5
nor x7, x4, x5
nor x2, x6, x7
nor x2, x2, x2
li x9, 8
bit:
cbit x4, x2, 31
beq x4, x2, shift_only
add x2, x2, x2
nor x5, x2, x10
nor x6, x2, x5
nor x7, x10, x5
nor x2, x6, x7
nor x2, x2, x2
j next_bit
shift_only:
add x2, x2, x2
next_bit:
addi x9, x9, -1
beq x9, x0, next_byte
j bit
next_byte:
addi x3, x3, 1
beq x3, x14, check
j byte
check:
beq x2, x11, next_message
j fail
next_message:
addi x1, x1, -1
beq x1, x0, success
j message
success:
li x0, 0
li x8, 93
syscall
fail:
li x0, 1
li x8, 93
syscall
