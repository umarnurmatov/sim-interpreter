li x1, 11
li x2, 29
li x3, 512
stp x1, x2, 0(x3)
ld x4, [x0, 512]
ld x5, [x0, 516]
add x4, x4, x4
add x6, x4, x5
li x1, -1
li x2, 2
li x3, 508
stp x1, x2, 4(x3)
ld x4, [x0, 512]
ld x5, [x0, 516]
add x4, x4, x4
add x6, x4, x5
li x1, -32768
li x2, 32767
li x3, 520
stp x1, x2, -8(x3)
ld x4, [x0, 512]
ld x5, [x0, 516]
add x4, x4, x4
add x6, x4, x5
li x1, 7
li x2, 13
li x3, 1016
stp x1, x2, 0(x3)
ld x4, [x0, 1016]
ld x5, [x0, 1020]
add x4, x4, x4
add x6, x4, x5
li x1, 512
li x2, 19
stp x1, x2, 0(x1)
ld x4, [x0, 512]
ld x5, [x0, 516]
add x4, x4, x4
add x6, x4, x5
