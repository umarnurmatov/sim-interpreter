#include "thread_executor.hpp"

#include "bitops.hpp"

ThreadExecutor::ThreadExecutor()
  : m_handlers {
      &ThreadExecutor::exec_unknown,
      &ThreadExecutor::exec_bdep,
      &ThreadExecutor::exec_nor,
      &ThreadExecutor::exec_cls,
      &ThreadExecutor::exec_syscall,
      &ThreadExecutor::exec_add,
      &ThreadExecutor::exec_ssat,
      &ThreadExecutor::exec_beq,
      &ThreadExecutor::exec_ld,
      &ThreadExecutor::exec_cbit,
      &ThreadExecutor::exec_j,
      &ThreadExecutor::exec_addi,
      &ThreadExecutor::exec_jalr,
      &ThreadExecutor::exec_st,
      &ThreadExecutor::exec_stp,
      &ThreadExecutor::exec_li
    }
{
}

int ThreadExecutor::run()
{
  int exit_code = 1;

  try {
    exec(Instr{ .opc = Opcode::kUnknown });
  }
  catch (const SyscallTrap &trap) {
    switch (trap.num) {
      case Isa::kSyscallExit:
        exit_code = trap.args[0];
        break;
      default:
        throw std::runtime_error("interpreter: unknown syscall");
    }
  }

  return exit_code;
}

#define OPC_TO_INT(opc) \
  (static_cast<std::uint8_t>(opc))

#if defined(__GNUC__) || defined(__clang__)
#define DISPATCH()                   \
  Instr inst_next = decode(fetch()); \
  [[clang::musttail]]                \
  return                             \
  ((this->*m_handlers[OPC_TO_INT(inst_next.opc)])(inst_next));
#else
#error "musttail must be specified"
#endif

void ThreadExecutor::exec(Instr inst)
{
  DISPATCH();
}

void ThreadExecutor::exec_unknown(Instr inst)
{
  throw std::runtime_error("exec: unknown instruction");
}

void ThreadExecutor::exec_bdep(Instr inst)
{
  m_cpu->set_reg(
    inst.f3,
    bit_deposit(
      m_cpu->get_reg(inst.f2),
      m_cpu->get_reg(inst.f1))
  );
  m_cpu->increment_pc(sizeof(Word));

  DISPATCH();
}

void ThreadExecutor::exec_nor(Instr inst)
{
  Reg res = ~(m_cpu->get_reg(inst.f2)
              | m_cpu->get_reg(inst.f3));
  m_cpu->set_reg(inst.f1, res);
  m_cpu->increment_pc(sizeof(Word));

  DISPATCH();
}

void ThreadExecutor::exec_cls(Instr inst)
{
  Reg src = m_cpu->get_reg(inst.f1);
  std::size_t ones_cnt = cnt_lead_ones(src);
  m_cpu->set_reg(inst.f2, ones_cnt);
  m_cpu->increment_pc(sizeof(Word));

  DISPATCH();
}

void ThreadExecutor::exec_syscall(Instr inst)
{
  SyscallTrap trap {};
  trap.num = m_cpu->get_reg(Isa::x8);

  for (std::size_t i = 0; i < Isa::kSyscallArgCnt; ++i)
    trap.args[i] = m_cpu->get_reg(i);

  m_cpu->increment_pc(sizeof(Word));

  throw trap;
}

void ThreadExecutor::exec_add(Instr inst)
{
  Reg res = m_cpu->get_reg(inst.f2) +
            m_cpu->get_reg(inst.f3);
  m_cpu->set_reg(inst.f1, res);
  m_cpu->increment_pc(sizeof(Word));

  DISPATCH();
}

void ThreadExecutor::exec_ssat(Instr inst)
{
  Reg src = m_cpu->get_reg(inst.f1);
  Reg sgn_satred = sgn_satr(src, inst.f3);
  m_cpu->set_reg(inst.f2, sgn_satred);
  m_cpu->increment_pc(sizeof(Word));

  DISPATCH();
}

void ThreadExecutor::exec_beq(Instr inst)
{
  // FIXME
  Word ofst = sgn_extend(
    inst.f3 << 2,
    kInstrEnc[OPC_TO_INT(Opcode::kBeq)].f3.width + 2);

  Reg src   = m_cpu->get_reg(inst.f2),
      targt = m_cpu->get_reg(inst.f1);
  m_cpu->increment_pc(src == targt ? ofst : sizeof(Word));

  DISPATCH();
}

void ThreadExecutor::exec_ld(Instr inst)
{
  Word ofst = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kLd)].f3.width);

  Word addr = m_cpu->get_reg(inst.f2) + static_cast<Word>(ofst);

  m_cpu->set_reg(inst.f1, m_cpu->mem().load<Word>(addr));
  m_cpu->increment_pc(sizeof(Word));

  DISPATCH();
}

void ThreadExecutor::exec_cbit(Instr inst)
{
  Reg src = m_cpu->get_reg(inst.f2);
  clr_bit(src, inst.f3);
  m_cpu->set_reg(inst.f1, src);
  m_cpu->increment_pc(sizeof(Word));

  DISPATCH();
}

void ThreadExecutor::exec_j(Instr inst)
{
  m_cpu->set_pc(
    (m_cpu->pc() & 0xf0000000) | (inst.f3 << 2));

  DISPATCH();
}

void ThreadExecutor::exec_addi(Instr inst)
{
  Word imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kAddi)].f3.width);

  Reg res = m_cpu->get_reg(inst.f2) + static_cast<Reg>(imm);
  m_cpu->set_reg(inst.f1, res);
  m_cpu->increment_pc(sizeof(Word));

  DISPATCH();
}

void ThreadExecutor::exec_jalr(Instr inst)
{
  Reg link = m_cpu->pc() + sizeof(Word);
  Word imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kJalr)].f3.width);
  Reg src = m_cpu->get_reg(inst.f2);
  m_cpu->set_pc((src + imm) & 0xfffffffe);
  m_cpu->set_reg(inst.f1, link);

  DISPATCH();
}

void ThreadExecutor::exec_st(Instr inst)
{
  Word ofst = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kLd)].f3.width);

  Word addr = m_cpu->get_reg(inst.f2) + static_cast<Word>(ofst);

  m_cpu->mem().store(addr, m_cpu->get_reg(inst.f1));
  m_cpu->increment_pc(sizeof(Word));

  DISPATCH();
}

void ThreadExecutor::exec_stp(Instr inst)
{
  auto f4_width = kInstrEnc[OPC_TO_INT(Opcode::kStp)].f4.width;

  Word base = inst.f3 >> f4_width;
  Word ofst = inst.f3 & ((1u << f4_width) - 1);

  Word ofst_ext = sgn_extend(ofst, f4_width);

  Word addr = m_cpu->get_reg(base) + static_cast<Word>(ofst_ext);

  m_cpu->mem().store(addr               , m_cpu->get_reg(inst.f1));
  m_cpu->mem().store(addr + sizeof(Word), m_cpu->get_reg(inst.f2));
  m_cpu->increment_pc(sizeof(Word));

  DISPATCH();
}

void ThreadExecutor::exec_li(Instr inst)
{
  Word imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kLi)].f3.width);
  m_cpu->set_reg(inst.f1, std::bit_cast<Reg>(imm));
  m_cpu->increment_pc(sizeof(Word));

  DISPATCH();
}

#undef OPC_TO_INT
#undef DISPATCH
