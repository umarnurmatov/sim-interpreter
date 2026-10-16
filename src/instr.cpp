#include "instr.hpp"

Opcode get_opcode(Word enc)
{
  std::uint8_t opcode_lsb = static_cast<std::uint8_t>(enc & 0x3f);
  std::uint8_t opcode_msb = static_cast<std::uint8_t>(enc >> 26);
  Opcode op{};

  if (opcode_lsb && !opcode_msb) {
    switch (opcode_lsb) {
      case Isa::kOpcodeBdep:    op = Opcode::kBdep;    break;
      case Isa::kOpcodeNor:     op = Opcode::kNor;     break;
      case Isa::kOpcodeCls:     op = Opcode::kCls;     break;
      case Isa::kOpcodeSyscall: op = Opcode::kSyscall; break;
      case Isa::kOpcodeAdd:     op = Opcode::kAdd;     break;
      default:                  op = Opcode::kUnknown; break;
    }
  }
  else {
    switch (opcode_msb) {
      case Isa::kOpcodeSsat:    op = Opcode::kSsat;    break;
      case Isa::kOpcodeBeq:     op = Opcode::kBeq;     break;
      case Isa::kOpcodeLd:      op = Opcode::kLd;      break;
      case Isa::kOpcodeCbit:    op = Opcode::kCbit;    break;
      case Isa::kOpcodeJ:       op = Opcode::kJ;       break;
      case Isa::kOpcodeAddi:    op = Opcode::kAddi;    break;
      case Isa::kOpcodeJalr:    op = Opcode::kJalr;    break;
      case Isa::kOpcodeSt:      op = Opcode::kSt;      break;
      case Isa::kOpcodeStp:     op = Opcode::kStp;     break;
      case Isa::kOpcodeLi:      op = Opcode::kLi;      break;
      default:                  op = Opcode::kUnknown; break;
    }
  }

  return op;
}
