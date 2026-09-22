#include "cpu.hpp"

#include "opcodes.h"

static Opcode get_opcode(Word &enc);

Cpu::Cpu()
{
  m_cpu = new CpuState;
}

Cpu::~Cpu()
{
  delete m_cpu;
}

#define GET_SRC1(enc)   (static_cast<Byte>(((enc) >> 16) & 0x1f     ))
#define GET_SRC2(enc)   (static_cast<Byte>(((enc) >> 11) & 0x1f     ))
#define GET_DST(enc)    (static_cast<Byte>(((enc) >> 21) & 0x1f     ))
#define GET_TARGT1(enc) (static_cast<Byte>(((enc) >> 16) & 0x1f     ))
#define GET_TARGT2(enc) (static_cast<Byte>(((enc) >> 11) & 0x1f     ))
#define GET_IMM5(enc)   (static_cast<Word>(((enc) >> 11) & 0x1f     ))
#define GET_IMM14(enc)  (static_cast<Word>( (enc)        & 0x7fff   ))
#define GET_IMM16(enc)  (static_cast<Word>( (enc)        & 0xffff   ))
#define GET_OFST16(enc) (static_cast<Word>( (enc)        & 0xffff   ))
#define GET_OFST11(enc) (static_cast<Word>( (enc)        & 0x7ff    ))
#define GET_BASE(enc)   (static_cast<Byte>(((enc) >> 21) & 0x1f     ))
#define GET_IIND(enc)   (static_cast<Word>( (enc)        & 0x3ffffff))
#define GET_CODE(enc)   (static_cast<Word>(((enc) >>  6) & 0xfffff  ))


Instr Cpu::decode(Word &enc)
{
  Instr inst { .opc = get_opcode(enc) };

  switch (inst.opc) {
    // Opcode in [5:0]
    case Opcode::kBdep:
    case Opcode::kNor:
      inst.f1 = GET_DST   (enc);
      inst.f2 = GET_SRC1  (enc);
      inst.f3 = GET_SRC2  (enc);
      break;

    case Opcode::kAdd:
      // in ADD instruction dst and src are located same place 
      // as src2 and dst respectively in Bdep and Nor (Why???)
      inst.f1 = GET_SRC2  (enc); 
      inst.f2 = GET_TARGT1(enc);
      inst.f3 = GET_DST   (enc); 
      break;

    case Opcode::kSsat:
      inst.f1 = GET_DST   (enc);
      inst.f2 = GET_SRC1  (enc);
      inst.f3 = GET_IMM5  (enc);
      break;

    case Opcode::kCls:
      inst.f1 = GET_DST   (enc);
      inst.f2 = GET_SRC1  (enc);
      break;
      
    case Opcode::kSyscall:
      inst.f3 = GET_CODE  (enc);
      break;

    // Opcode in [31:26]
    case Opcode::kBeq:
      inst.f1 = GET_DST   (enc);
      inst.f2 = GET_SRC1  (enc);
      inst.f3 = GET_OFST16(enc);
      break;

    case Opcode::kLd:
    case Opcode::kLi:
      inst.f1 = GET_BASE  (enc);
      inst.f2 = GET_TARGT1(enc);
      inst.f3 = GET_IMM14 (enc);
      break;

    case Opcode::kCbit:
      inst.f1 = GET_DST   (enc);
      inst.f2 = GET_SRC1  (enc);
      inst.f3 = GET_IMM5  (enc);
      break;

    case Opcode::kJ:
      inst.f3 = GET_IIND  (enc);
      break;

    case Opcode::kAddi:
    case Opcode::kJalr:
      inst.f1 = GET_DST   (enc); // rd
      inst.f2 = GET_TARGT1(enc);
      inst.f3 = GET_IMM16 (enc);
      break;

    case Opcode::kStp:
      inst.f1 = GET_BASE  (enc);
      inst.f2 = GET_TARGT1(enc);
      inst.f3 = GET_TARGT2(enc);
      break;

    default:
      break;
  }

  return inst;
}

#undef GET_SRC1
#undef GET_SRC2
#undef GET_DST
#undef GET_TARGT1
#undef GET_IMM5
#undef GET_IMM14
#undef GET_IMM16
#undef GET_OFST16
#undef GET_OFST11
#undef GET_BASE
#undef GET_IIND
#undef GET_CODE

void Cpu::exec(Instr &inst)
{
  Reg res{};
  switch (inst.opc) {
    case Opcode::kBdep:
    case Opcode::kNor:
      res = m_cpu->get_reg(inst.get_src1()) +
            m_cpu->get_reg(inst.get_src2());
      m_cpu->set_reg(inst.get_dst(), res);
      break;
    case Opcode::kCls:
    case Opcode::kSyscall:
    case Opcode::kAdd:
    case Opcode::kSsat:
    case Opcode::kBeq:
    case Opcode::kLd:
    case Opcode::kCbit:
    case Opcode::kJ:
    case Opcode::kAddi:
    case Opcode::kJalr:
    case Opcode::kSt:
    case Opcode::kStp:
    case Opcode::kLi:
    default:
      break;
  }
}

static Opcode get_opcode(Word &enc)
{
  std::uint8_t opcode_lsb = static_cast<std::uint8_t>(enc & 0x3f);
  std::uint8_t opcode_msb = static_cast<std::uint8_t>(enc >> 26);
  Opcode op{};

  if (opcode_lsb && !opcode_msb) {
    switch (opcode_lsb) {
      case OPCODE_BDEP:    op = Opcode::kBdep;
      case OPCODE_NOR:     op = Opcode::kNor;
      case OPCODE_CLS:     op = Opcode::kCls;
      case OPCODE_SYSCALL: op = Opcode::kSyscall;
      case OPCODE_ADD:     op = Opcode::kAdd;
      default:             op = Opcode::kUnknown;
    }
  }
  else {
    switch (opcode_msb) {
      case OPCODE_SSAT:    op = Opcode::kSsat;
      case OPCODE_BEQ:     op = Opcode::kBeq;
      case OPCODE_LD:      op = Opcode::kLd;
      case OPCODE_CBIT:    op = Opcode::kCbit;
      case OPCODE_J:       op = Opcode::kJ;
      case OPCODE_ADDI:    op = Opcode::kAddi;
      case OPCODE_JALR:    op = Opcode::kJalr;
      case OPCODE_ST:      op = Opcode::kSt;
      case OPCODE_STP:     op = Opcode::kStp;
      case OPCODE_LI:      op = Opcode::kLi;
      default:             op = Opcode::kUnknown;
    }
  }

  return op;
}


