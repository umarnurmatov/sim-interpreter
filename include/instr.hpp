#pragma once
#include <cstdint>

using Word = std::uint32_t;
using Byte = std::uint8_t;

enum class Opcode : std::uint8_t
{
  kUnknown = 0,

  // Opcode in [5:0]
  kBdep,
  kNor,
  kCls,
  kSyscall,
  kAdd,

  // Opcode in [31:26]
  kSsat,
  kBeq,
  kLd,
  kCbit,
  kJ,
  kAddi,
  kJalr,
  kSt,
  kStp,
  kLi
};

struct Instr
{
  Opcode opc{};

  Byte f1{},  // dst,  base, targt1
       f2{};  // src1, targt2
  Word f3{};  // src2, imm, ofst, code

  Byte &get_src1() { return f2; }
  Word &get_src2() { return f3; }
  Byte &get_dst () { return f1; }
  Word &get_imm () { return f3; }
  Byte &get_ofst() { return f2; }
};
