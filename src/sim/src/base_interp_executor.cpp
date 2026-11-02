#include "base_interp_executor.hpp"

#include "bitops.hpp"

#define OPC_TO_INT(opc) \
  (static_cast<std::uint8_t>(opc))

void BaseInterpExecutor::exec(Instr inst)
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


void BaseInterpExecutor::exec_bdep(Instr inst)
{
  m_cpu->set_reg(
    inst.f3,
    bit_deposit(
      m_cpu->get_reg(inst.f2),
      m_cpu->get_reg(inst.f1))
  );
  m_cpu->increment_pc(sizeof(Word));
}

void BaseInterpExecutor::exec_nor(Instr inst)
{
  Reg res = ~(m_cpu->get_reg(inst.f2)
              | m_cpu->get_reg(inst.f3));
  m_cpu->set_reg(inst.f1, res);
  m_cpu->increment_pc(sizeof(Word));
}

void BaseInterpExecutor::exec_cls(Instr inst)
{
  Reg src = m_cpu->get_reg(inst.f1);
  std::size_t ones_cnt = cnt_lead_ones(src);
  m_cpu->set_reg(inst.f2, ones_cnt);
  m_cpu->increment_pc(sizeof(Word));
}

void BaseInterpExecutor::exec_syscall(Instr)
{
  SyscallTrap trap {};
  trap.num = m_cpu->get_reg(Isa::x8);

  for (std::size_t i = 0; i < Isa::kSyscallArgCnt; ++i)
    trap.args[i] = m_cpu->get_reg(i);

  m_cpu->increment_pc(sizeof(Word));

  throw trap;
}

void BaseInterpExecutor::exec_add(Instr inst)
{
  Reg res = m_cpu->get_reg(inst.f2) +
            m_cpu->get_reg(inst.f3);
  m_cpu->set_reg(inst.f1, res);
  m_cpu->increment_pc(sizeof(Word));
}

void BaseInterpExecutor::exec_ssat(Instr inst)
{
  Reg src = m_cpu->get_reg(inst.f1);
  Reg sgn_satred = sgn_satr(src, inst.f3);
  m_cpu->set_reg(inst.f2, sgn_satred);
  m_cpu->increment_pc(sizeof(Word));
}

void BaseInterpExecutor::exec_beq(Instr inst)
{
  // FIXME
  Word ofst = sgn_extend(
    inst.f3 << 2,
    kInstrEnc[OPC_TO_INT(Opcode::kBeq)].f3.width + 2);

  Reg src   = m_cpu->get_reg(inst.f2),
      targt = m_cpu->get_reg(inst.f1);
  m_cpu->increment_pc(src == targt ? ofst : sizeof(Word));
}

void BaseInterpExecutor::exec_ld(Instr inst)
{
  Word ofst = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kLd)].f3.width);

  Word addr = m_cpu->get_reg(inst.f2) + static_cast<Word>(ofst);

  m_cpu->set_reg(inst.f1, m_cpu->mem().load<Word>(addr));
  m_cpu->increment_pc(sizeof(Word));
}

void BaseInterpExecutor::exec_cbit(Instr inst)
{
  Reg src = m_cpu->get_reg(inst.f2);
  clr_bit(src, inst.f3);
  m_cpu->set_reg(inst.f1, src);
  m_cpu->increment_pc(sizeof(Word));
}

void BaseInterpExecutor::exec_j(Instr inst)
{
  m_cpu->set_pc(
    (m_cpu->pc() & 0xf0000000) | (inst.f3 << 2));
}

void BaseInterpExecutor::exec_addi(Instr inst)
{
  Word imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kAddi)].f3.width);

  Reg res = m_cpu->get_reg(inst.f2) + static_cast<Reg>(imm);
  m_cpu->set_reg(inst.f1, res);
  m_cpu->increment_pc(sizeof(Word));
}

void BaseInterpExecutor::exec_jalr(Instr inst)
{
  Reg link = m_cpu->pc() + sizeof(Word);
  Word imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kJalr)].f3.width);
  Reg src = m_cpu->get_reg(inst.f2);
  m_cpu->set_pc((src + imm) & 0xfffffffe);
  m_cpu->set_reg(inst.f1, link);
}

void BaseInterpExecutor::exec_st(Instr inst)
{
  Word ofst = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kLd)].f3.width);

  Word addr = m_cpu->get_reg(inst.f2) + static_cast<Word>(ofst);

  m_cpu->mem().store(addr, m_cpu->get_reg(inst.f1));
  m_cpu->increment_pc(sizeof(Word));
}

void BaseInterpExecutor::exec_stp(Instr inst)
{
  auto f4_width = kInstrEnc[OPC_TO_INT(Opcode::kStp)].f4.width;

  Word base = inst.f3 >> f4_width;
  Word ofst = inst.f3 & ((1u << f4_width) - 1);

  Word ofst_ext = sgn_extend(ofst, f4_width);

  Word addr = m_cpu->get_reg(base) + static_cast<Word>(ofst_ext);

  m_cpu->mem().store(addr               , m_cpu->get_reg(inst.f1));
  m_cpu->mem().store(addr + sizeof(Word), m_cpu->get_reg(inst.f2));
  m_cpu->increment_pc(sizeof(Word));
}

void BaseInterpExecutor::exec_li(Instr inst)
{
  Word imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kLi)].f3.width);
  m_cpu->set_reg(inst.f1, std::bit_cast<Reg>(imm));
  m_cpu->increment_pc(sizeof(Word));
}

#undef OPC_TO_INT
