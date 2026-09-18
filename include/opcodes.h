#pragma once

// Opcode in [5:0]
#define OPCODE_BDEP    0b011110
#define OPCODE_NOR     0b001010
#define OPCODE_CLS     0b110111
#define OPCODE_SYSCALL 0b100011
#define OPCODE_ADD     0b011011

// Opcode in [31:26]
#define OPCODE_SSAT    0b100000
#define OPCODE_BEQ     0b010010
#define OPCODE_LD      0b000010
#define OPCODE_CBIT    0b011111
#define OPCODE_J       0b110010
#define OPCODE_ADDI    0b111011
#define OPCODE_JALR    0b101001
#define OPCODE_ST      0b101100
#define OPCODE_STP     0b111000
#define OPCODE_LI      0b011001
