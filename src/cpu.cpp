#include "cpu.hpp"

#include <algorithm>
#include <bit>
#include <stdexcept>

#include "immintrin.h"

static Opcode get_opcode(Word enc);
static std::size_t cnt_lead_ones(Reg rg);
static Reg sgn_satr(Reg rg, Word n);
static SignedWord sgn_extend(Word wd, Word bitw);
static void clr_bit(Word &wd, std::uint8_t pos);
static Reg bit_deposit(Reg rg, Reg mask);

Cpu::Cpu()
{
  m_cpu = new CpuState {};
}

Cpu::~Cpu()
{
  delete m_cpu;
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

Instr Cpu::decode(Word enc)
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
      GET_INSTR_REG3_REG2OFFS(OPC_TO_INT(Opcode::kNor), enc);
      break;
    case Opcode::kJalr:
      GET_INSTR_REG3_REG2OFFS(OPC_TO_INT(Opcode::kNor), enc);
      break;

    case Opcode::kCls:
      GET_INSTR_REG2(OPC_TO_INT(Opcode::kCls), enc);
      break;
      
    case Opcode::kSyscall:
      GET_INSTR_IMM(OPC_TO_INT(Opcode::kSyscall), enc);
      break;

    case Opcode::kJ:
      GET_INSTR_IMM(OPC_TO_INT(Opcode::kJ), enc);
      break;

    case Opcode::kStp: {
      GET_INSTR_STP(enc);
      break;
    }

    default:
      throw std::runtime_error("decode: unknown instruction");
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

void Cpu::exec(Instr inst)
{

  switch (inst.opc) {
    case Opcode::kBdep:
      m_cpu->set_reg(
        inst.f3, 
        bit_deposit(
          m_cpu->get_reg(inst.f1), 
          m_cpu->get_reg(inst.f2))
      );
      break;

    case Opcode::kNor: {
      Reg res = ~(m_cpu->get_reg(inst.f2) 
                  | m_cpu->get_reg(inst.f3));
      m_cpu->set_reg(inst.f1, res);
      break;
    }

    case Opcode::kCls: {
      Reg src = m_cpu->get_reg(inst.f1);
      std::size_t ones_cnt = cnt_lead_ones(src);
      m_cpu->set_reg(inst.f2, ones_cnt);
      break;
    }

    case Opcode::kSyscall:
      break;
      
    case Opcode::kAdd: {
      Reg res = m_cpu->get_reg(inst.f2) +
                m_cpu->get_reg(inst.f3);
      m_cpu->set_reg(inst.f1, res);
      break;
    }

    case Opcode::kSsat: {
      Reg src = m_cpu->get_reg(inst.f1); 
      Reg sgn_satred = sgn_satr(src, inst.f3);
      m_cpu->set_reg(inst.f2, sgn_satred);
    }

    case Opcode::kBeq: {
      // FIXME
      SignedWord ofst = sgn_extend(
        inst.f3 << 2,
        kInstrEnc[OPC_TO_INT(Opcode::kBeq)].f3.width + 2);

      Reg src   = m_cpu->get_reg(inst.f2),
          targt = m_cpu->get_reg(inst.f1);
      m_cpu->increment_pc(src == targt ? ofst : sizeof(Word));
    }

    case Opcode::kLd: {
      SignedWord ofst = sgn_extend(
        inst.f3,
        kInstrEnc[OPC_TO_INT(Opcode::kLd)].f3.width);

      Word addr = std::bit_cast<Word>(
        std::bit_cast<SignedWord>(m_cpu->get_reg(inst.f2)) 
        + ofst);

      m_cpu->set_reg(inst.f1, m_cpu->mem().load<Word>(addr));
      break;
    }

    case Opcode::kCbit: {
      Reg src = m_cpu->get_reg(inst.f2);
      clr_bit(src, inst.f3);
      m_cpu->set_reg(inst.f1, src);
      break;
    }

    case Opcode::kJ: {
      m_cpu->increment_pc(
        (m_cpu->pc() & 0xf0000000) | (inst.f3 << 2));
      break;
    }

    case Opcode::kAddi: {
      SignedWord imm = sgn_extend(
        inst.f3,
        kInstrEnc[OPC_TO_INT(Opcode::kLd)].f3.width);
      break;

      Reg res = std::bit_cast<Reg>(
        std::bit_cast<SignedWord>(m_cpu->get_reg(inst.f2))
        + imm
      );
      m_cpu->set_reg(inst.f1, res);
    }

    case Opcode::kJalr: {
      Reg link = m_cpu->pc() + sizeof(Word);
      SignedWord imm = sgn_extend(
        inst.f3,
        kInstrEnc[OPC_TO_INT(Opcode::kJalr)].f3.width);
      Reg src = m_cpu->get_reg(inst.f2);
      m_cpu->set_pc((src + imm) & 0xfffffffe);
      m_cpu->set_reg(inst.f1, link);
      break;
    }
     
    case Opcode::kSt: {
      SignedWord ofst = sgn_extend(
        inst.f3,
        kInstrEnc[OPC_TO_INT(Opcode::kLd)].f3.width);

      Word addr = std::bit_cast<Word>(
        std::bit_cast<SignedWord>(m_cpu->get_reg(inst.f2)) 
        + ofst);

      m_cpu->mem().store(addr, m_cpu->get_reg(inst.f1));
      break;
    }
       
    case Opcode::kStp: {
      auto f4_width = kInstrEnc[OPC_TO_INT(Opcode::kStp)].f4.width;

      Word base = inst.f3 >> f4_width;
      Word ofst = inst.f3 & ((1u << f4_width) - 1);

      SignedWord ofst_ext = sgn_extend(ofst, f4_width);

      Word addr = std::bit_cast<Word>(
        std::bit_cast<SignedWord>(m_cpu->get_reg(base)) 
        + ofst_ext);

      m_cpu->mem().store(addr               , m_cpu->get_reg(inst.f1));
      m_cpu->mem().store(addr + sizeof(Word), m_cpu->get_reg(inst.f2));
      break;
    }
       
    case Opcode::kLi: {
      SignedWord imm = sgn_extend(
        inst.f3,
        kInstrEnc[OPC_TO_INT(Opcode::kLd)].f3.width);
      m_cpu->set_reg(inst.f1, std::bit_cast<Reg>(imm));
      break;
    }
       
    default:
      throw std::runtime_error("exec: unknown instr");
      break;
  }
}

#undef OPC_TO_INT

static Opcode get_opcode(Word enc)
{
  std::uint8_t opcode_lsb = static_cast<std::uint8_t>(enc & 0x3f);
  std::uint8_t opcode_msb = static_cast<std::uint8_t>(enc >> 26);
  Opcode op{};

  if (opcode_lsb && !opcode_msb) {
    switch (opcode_lsb) {
      case kOpcodeBdep:    op = Opcode::kBdep;    break;
      case kOpcodeNor:     op = Opcode::kNor;     break;
      case kOpcodeCls:     op = Opcode::kCls;     break;
      case kOpcodeSyscall: op = Opcode::kSyscall; break;
      case kOpcodeAdd:     op = Opcode::kAdd;     break;
      default:             op = Opcode::kUnknown; break;
    }
  }
  else {
    switch (opcode_msb) {
      case kOpcodeSsat:    op = Opcode::kSsat;    break;
      case kOpcodeBeq:     op = Opcode::kBeq;     break;
      case kOpcodeLd:      op = Opcode::kLd;      break;
      case kOpcodeCbit:    op = Opcode::kCbit;    break;
      case kOpcodeJ:       op = Opcode::kJ;       break;
      case kOpcodeAddi:    op = Opcode::kAddi;    break;
      case kOpcodeJalr:    op = Opcode::kJalr;    break;
      case kOpcodeSt:      op = Opcode::kSt;      break;
      case kOpcodeStp:     op = Opcode::kStp;     break;
      case kOpcodeLi:      op = Opcode::kLi;      break;
      default:             op = Opcode::kUnknown; break;
    }
  }

  return op;
}

static std::size_t cnt_lead_ones(Reg rg)
{
  #if defined(__GNUC__)
    return rg == UINT32_MAX 
                 ? kRegWidth 
                 : static_cast<std::size_t>(__builtin_clz(~rg));
  #else
    std::size_t cnt = 0;
    while (rg & (1u << (kRegWidth - 1))) {
        ++n;
        rg <<= 1;
    }
    return cnt;
  #endif
}

// assuming n <= 31
static Reg sgn_satr(Reg rg, Word n)
{
  return std::clamp<Reg>(rg, -(1 << (n-1)), (1 << (n-1)) - 1);
}

// assuming bitw <= 31
static SignedWord sgn_extend(Word wd, Word bitw)
{
  const Word m = 1u << (bitw - 1);   
  return std::bit_cast<SignedWord>((wd ^ m) - m);
}

static void clr_bit(Word &wd, std::uint8_t pos)
{
  wd &= ~(1u << pos);
}

static Reg bit_deposit(Reg rg, Reg mask)
{
  if (__builtin_cpu_supports("bmi2")) {
    return _pdep_u32(rg, mask);
  }

  Reg res {};
  for (std::uint32_t bit = 1; mask; bit <<= 1) {
      std::uint32_t mask_lsb = mask & -mask;
      if (rg & bit)
          res |= mask_lsb;
      mask &= mask - 1;
  }
  return res; 
}
