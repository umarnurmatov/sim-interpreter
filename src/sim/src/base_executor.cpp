#include "base_executor.hpp"

#include <cassert>

#include "isa.hpp"

BaseExecutor::BaseExecutor()
{
  m_cpu = new CpuState {};
}

BaseExecutor::~BaseExecutor()
{
  delete m_cpu;
}

Word BaseExecutor::fetch()
{
  return m_cpu->mem().load<Word>(m_cpu->pc());
}

#define GET_FIELD_VAL(enc, width, offset) \
  ((enc >> offset) & ((1u << width) - 1))

#define GET_FIELD(enc, field_enc) \
  (GET_FIELD_VAL(enc, field_enc.width, field_enc.offset))

#define OPC_TO_INT(opc) \
  (static_cast<std::uint8_t>(opc))

#define GET_INSTR_REG3_REG2OFFS(opc, enc)       \
  inst.f1 = GET_FIELD(enc, kInstrEnc[opc].f1);  \
  inst.f2 = GET_FIELD(enc, kInstrEnc[opc].f2);  \
  inst.f3 = GET_FIELD(enc, kInstrEnc[opc].f3);  \

#define GET_INSTR_REG2(opc, enc)                \
  inst.f1 = GET_FIELD(enc, kInstrEnc[opc].f1);  \
  inst.f2 = GET_FIELD(enc, kInstrEnc[opc].f2);  \

#define GET_INSTR_IMM(opc, enc)                 \
  inst.f3 = GET_FIELD(enc, kInstrEnc[opc].f3);  \

#define GET_INSTR_STP(enc)                        \
  std::uint8_t opc = OPC_TO_INT(Opcode::kStp);    \
  inst.f1 = GET_FIELD(enc, kInstrEnc[opc].f1);    \
  inst.f2 = GET_FIELD(enc, kInstrEnc[opc].f2);    \
  Word f3 = GET_FIELD(enc, kInstrEnc[opc].f3);    \
  Word f4 = GET_FIELD(enc, kInstrEnc[opc].f4);    \
  inst.f3 = (f3 << kInstrEnc[opc].f4.width) | f4; \

Instr BaseExecutor::decode(Word enc)
{
  Instr inst { .opc = get_opcode(enc) };

  switch (inst.opc) {
    case Opcode::kBdep:
      GET_INSTR_REG3_REG2OFFS(OPC_TO_INT(Opcode::kBdep), enc);
      break;
    case Opcode::kNor:
      GET_INSTR_REG3_REG2OFFS(OPC_TO_INT(Opcode::kNor), enc);
      break;
    case Opcode::kAdd:
      GET_INSTR_REG3_REG2OFFS(OPC_TO_INT(Opcode::kAdd), enc);
      break;
    case Opcode::kSsat:
      GET_INSTR_REG3_REG2OFFS(OPC_TO_INT(Opcode::kSsat), enc);
      break;
    case Opcode::kBeq:
      GET_INSTR_REG3_REG2OFFS(OPC_TO_INT(Opcode::kBeq), enc);
      break;
    case Opcode::kLd:
      GET_INSTR_REG3_REG2OFFS(OPC_TO_INT(Opcode::kLd), enc);
      break;
    case Opcode::kLi:
      GET_INSTR_REG3_REG2OFFS(OPC_TO_INT(Opcode::kLi), enc);
      break;
    case Opcode::kCbit:
      GET_INSTR_REG3_REG2OFFS(OPC_TO_INT(Opcode::kCbit), enc);
      break;
    case Opcode::kAddi:
      GET_INSTR_REG3_REG2OFFS(OPC_TO_INT(Opcode::kAddi), enc);
      break;
    case Opcode::kJalr:
      GET_INSTR_REG3_REG2OFFS(OPC_TO_INT(Opcode::kJalr), enc);
      break;
    case Opcode::kSt:
      GET_INSTR_REG3_REG2OFFS(OPC_TO_INT(Opcode::kSt), enc);
      break;
    case Opcode::kCls:
      GET_INSTR_REG2(OPC_TO_INT(Opcode::kCls), enc);
      break;
    case Opcode::kSyscall:
      break;
    case Opcode::kJ:
      GET_INSTR_IMM(OPC_TO_INT(Opcode::kJ), enc);
      break;
    case Opcode::kStp: {
      GET_INSTR_STP(enc);
      break;
    }
    default:
      break;
  }

  return inst;
}

#undef GET_FIELD_VAL
#undef GET_FIELD
#undef GET_INSTR_STP
#undef GET_INSTR_REG2
#undef GET_INSTR_IMM
#undef GET_INSTR_REG3_REG2OFFS

void BaseExecutor::load_binary(const std::vector<std::byte> &bin)
{
  m_cpu->mem().load_bytes(0, bin); 
  m_cpu->set_pc(0);
}
