li x1, 512
li x2, 123
st x2, [x1, 0]
ld x3, [x1, 0]
li x1, 512
li x2, -1
st x2, [x1, 0]
li x1, 508
ld x3, [x1, 4]
li x1, 512
li x2, -32768
st x2, [x1, 0]
li x1, 516
ld x3, [x1, -4]
li x1, 1020
li x2, 32767
st x2, [x1, 0]
ld x1, [x1, 0]
li x1, 0
li x2, 0
st x2, [x1, 0]
li x3, -1
ld x3, [x1, 0]
