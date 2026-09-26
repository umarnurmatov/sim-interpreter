#include "cpu.hpp"

#include <algorithm>
#include <bit>
#include <stdexcept>

#include "immintrin.h"

static Opcode get_opcode(Word enc);
static std::size_t cnt_lead_ones(Word rg);
static Word sgn_satr(Word rg, Word n);
static SignedWord sgn_extend(Word wd, Word bitw);
static void clr_bit(Word &wd, std::uint8_t pos);
static Word bit_deposit(Word rg, Word mask);

Cpu::Cpu()
{
  m_cpu = new CpuState {};
}

Cpu::~Cpu()
{
  delete m_cpu;
}


Word Cpu::fetch()
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
      exec_bdep(inst);
      break;
    case Opcode::kNor:
      exec_nor(inst);
      break;
    case Opcode::kCls:
      exec_cls(inst);
      break;
    case Opcode::kSyscall:
      exec_syscall(inst);
      break;
    case Opcode::kAdd:
      exec_add(inst);
      break;
    case Opcode::kSsat:
      exec_ssat(inst);
      break;
    case Opcode::kBeq:
      exec_beq(inst);
      break;
    case Opcode::kLd:
      exec_ld(inst);
      break;
    case Opcode::kCbit:
      exec_cbit(inst);
      break;
    case Opcode::kJ:
      exec_j(inst);
      break;
    case Opcode::kAddi:
      exec_addi(inst);
      break;
    case Opcode::kJalr:
      exec_jalr(inst);
      break;
    case Opcode::kSt:
      exec_st(inst);
      break;
    case Opcode::kStp:
      exec_stp(inst);
      break;
    case Opcode::kLi:
      exec_li(inst);
      break;
    default:
      throw std::runtime_error("exec: unknown instr");
  }
}

void Cpu::exec_block(BasicBlk blk)
{
  for(auto& instr : blk)
    exec(instr);
}

Reg Cpu::prefetch_basic_block(BasicBlk &blk)
{
  Reg pc_begin_blk = pc();
  Instr instr { .opc = Opcode::kUnknown };
  while (!IS_CONTROL_INSTRUCTION(instr)) {
    instr = decode(fetch());
    blk.push_back(instr);
    m_cpu->increment_pc(sizeof(Word));
  }
  m_cpu->set_pc(pc_begin_blk);
  return pc_begin_blk;
}

void Cpu::exec_bdep(Instr inst)
{
  m_cpu->set_reg(
    inst.f3,
    bit_deposit(
      m_cpu->get_reg(inst.f2),
      m_cpu->get_reg(inst.f1))
  );
  m_cpu->increment_pc(sizeof(Word));
}

void Cpu::exec_nor(Instr inst)
{
  Reg res = ~(m_cpu->get_reg(inst.f2)
              | m_cpu->get_reg(inst.f3));
  m_cpu->set_reg(inst.f1, res);
  m_cpu->increment_pc(sizeof(Word));
}

void Cpu::exec_cls(Instr inst)
{
  Reg src = m_cpu->get_reg(inst.f1);
  std::size_t ones_cnt = cnt_lead_ones(src);
  m_cpu->set_reg(inst.f2, ones_cnt);
  m_cpu->increment_pc(sizeof(Word));
}

void Cpu::exec_syscall(Instr)
{
  SyscallTrap trap {};
  trap.num = m_cpu->get_reg(Isa::x8);

  for (std::size_t i = 0; i < Isa::kSyscallArgCnt; ++i)
    trap.args[i] = m_cpu->get_reg(i);

  m_cpu->increment_pc(sizeof(Word));

  throw trap;
}

void Cpu::exec_add(Instr inst)
{
  Reg res = m_cpu->get_reg(inst.f2) +
            m_cpu->get_reg(inst.f3);
  m_cpu->set_reg(inst.f1, res);
  m_cpu->increment_pc(sizeof(Word));
}

void Cpu::exec_ssat(Instr inst)
{
  Reg src = m_cpu->get_reg(inst.f1);
  Reg sgn_satred = sgn_satr(src, inst.f3);
  m_cpu->set_reg(inst.f2, sgn_satred);
  m_cpu->increment_pc(sizeof(Word));
}

void Cpu::exec_beq(Instr inst)
{
  // FIXME
  SignedWord ofst = sgn_extend(
    inst.f3 << 2,
    kInstrEnc[OPC_TO_INT(Opcode::kBeq)].f3.width + 2);

  Reg src   = m_cpu->get_reg(inst.f2),
      targt = m_cpu->get_reg(inst.f1);
  m_cpu->increment_pc(src == targt ? ofst : sizeof(Word));
}

void Cpu::exec_ld(Instr inst)
{
  SignedWord ofst = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kLd)].f3.width);

  Word addr = m_cpu->get_reg(inst.f2) + static_cast<Word>(ofst);

  m_cpu->set_reg(inst.f1, m_cpu->mem().load<Word>(addr));
  m_cpu->increment_pc(sizeof(Word));
}

void Cpu::exec_cbit(Instr inst)
{
  Reg src = m_cpu->get_reg(inst.f2);
  clr_bit(src, inst.f3);
  m_cpu->set_reg(inst.f1, src);
  m_cpu->increment_pc(sizeof(Word));
}

void Cpu::exec_j(Instr inst)
{
  m_cpu->set_pc(
    (m_cpu->pc() & 0xf0000000) | (inst.f3 << 2));
}

void Cpu::exec_addi(Instr inst)
{
  SignedWord imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kAddi)].f3.width);

  Reg res = m_cpu->get_reg(inst.f2) + static_cast<Reg>(imm);
  m_cpu->set_reg(inst.f1, res);
  m_cpu->increment_pc(sizeof(Word));
}

void Cpu::exec_jalr(Instr inst)
{
  Reg link = m_cpu->pc() + sizeof(Word);
  SignedWord imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kJalr)].f3.width);
  Reg src = m_cpu->get_reg(inst.f2);
  m_cpu->set_pc((src + imm) & 0xfffffffe);
  m_cpu->set_reg(inst.f1, link);
}

void Cpu::exec_st(Instr inst)
{
  SignedWord ofst = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kLd)].f3.width);

  Word addr = m_cpu->get_reg(inst.f2) + static_cast<Word>(ofst);

  m_cpu->mem().store(addr, m_cpu->get_reg(inst.f1));
  m_cpu->increment_pc(sizeof(Word));
}

void Cpu::exec_stp(Instr inst)
{
  auto f4_width = kInstrEnc[OPC_TO_INT(Opcode::kStp)].f4.width;

  Word base = inst.f3 >> f4_width;
  Word ofst = inst.f3 & ((1u << f4_width) - 1);

  SignedWord ofst_ext = sgn_extend(ofst, f4_width);

  Word addr = m_cpu->get_reg(base) + static_cast<Word>(ofst_ext);

  m_cpu->mem().store(addr               , m_cpu->get_reg(inst.f1));
  m_cpu->mem().store(addr + sizeof(Word), m_cpu->get_reg(inst.f2));
  m_cpu->increment_pc(sizeof(Word));
}

void Cpu::exec_li(Instr inst)
{
  SignedWord imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kLi)].f3.width);
  m_cpu->set_reg(inst.f1, std::bit_cast<Reg>(imm));
  m_cpu->increment_pc(sizeof(Word));
}

#undef OPC_TO_INT

void Cpu::load_binary(std::vector<std::byte> &bin)
{
  m_cpu->mem().load_bytes(0, bin); 
}

static Opcode get_opcode(Word enc)
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

static std::size_t cnt_lead_ones(Word rg)
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
static Word sgn_satr(Word rg, Word n)
{
  SignedWord lower_b = -(1 << (n-1));
  SignedWord upper_b = (1 << (n-1)) - 1;
  SignedWord res = std::clamp<SignedWord>(
    std::bit_cast<SignedWord>(rg), 
    lower_b, 
    upper_b);

  return std::bit_cast<Word>(res);
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

static Word bit_deposit(Word rg, Word mask)
{
  if (__builtin_cpu_supports("bmi2")) {
    return _pdep_u32(rg, mask);
  }

  Word res {};
  for (std::uint32_t bit = 1; mask; bit <<= 1) {
      std::uint32_t mask_lsb = mask & -mask;
      if (rg & bit)
          res |= mask_lsb;
      mask &= mask - 1;
  }
  return res; 
}
