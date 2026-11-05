#include "jit_executor.hpp"
#include "asmjit/core/code_holder.h"
#include "asmjit/x86/x86_assembler.h"
#include "asmjit/x86/x86_operand.h"
#include "bitops.hpp"

JitExecutor::JitExecutor(std::size_t threshold)
  : m_threshold{ threshold }
{
}

JitExecutor::JitCompilerData::JitCompilerData(asmjit::x86::Compiler& cc_)
  : cc { cc_ }
{
  tmp1 = cc.new_gp32();
  tmp2 = cc.new_gp32();
  tmp3 = cc.new_gp32();

  pc_inc = cc.new_gp32();
  cc.mov(pc_inc, 0);
}

int JitExecutor::run()
{
  bool halted = false;
  int exit_code = 1;

  while (!halted) {
    auto bt_cache_it = m_bt_cache.find(pc());
    if (bt_cache_it != m_bt_cache.end()) {
      bt_cache_it->second();
      continue;
    }

    auto bb_cache_it = m_bb_cache.find(pc());

    if (bb_cache_it == m_bb_cache.end()) {
      BasicBlk blk;
      prefetch_basic_block(blk);
      bb_cache_it = m_bb_cache.insert({ pc(), blk }).first;
    }

    if (bb_cache_it->second.exec_cnt >= m_threshold) {
      JitBlk code = translate(bb_cache_it->second);
      m_bt_cache[pc()] = code;
      code();
    }
    else {
      bb_cache_it->second.exec_cnt++;
      interpret(bb_cache_it->second);
    }
  }

  return exit_code;
}

JitExecutor::JitBlk JitExecutor::translate(const BasicBlk& blk)
{
  asmjit::CodeHolder code; 
  code.init(m_runtime.environment(), 
            m_runtime.cpu_features());

  asmjit::x86::Compiler cc{ &code };

  JitCompilerData compiler_dat{ cc };

  cc.add_func(asmjit::FuncSignature::build<void>());

  for (const auto& instr : blk.instructions)
    translate_instr(instr, compiler_dat);

  JitBlk fn = nullptr;

  asmjit::Error cc_err = m_runtime.add(&fn, &code);
  if (cc_err != asmjit::Error::kOk) {
    // TODO
  }

  return fn;
}

#define CASE_TRANSLATE_INSTR_(cmd) \
  { translate_##cmd(inst, cdat); break; }

void JitExecutor::translate_instr(const Instr& inst,
                                  const JitCompilerData& cdat)
{
  switch(inst.opc) {
    case Opcode::kBdep:    CASE_TRANSLATE_INSTR_(bdep);
    case Opcode::kNor:     CASE_TRANSLATE_INSTR_(nor);
    case Opcode::kCls:     CASE_TRANSLATE_INSTR_(cls);
    case Opcode::kSyscall: CASE_TRANSLATE_INSTR_(syscall);
    case Opcode::kAdd:     CASE_TRANSLATE_INSTR_(add);
    case Opcode::kSsat:    CASE_TRANSLATE_INSTR_(ssat);
    case Opcode::kBeq:     CASE_TRANSLATE_INSTR_(beq);
    case Opcode::kLd:      CASE_TRANSLATE_INSTR_(ld);
    case Opcode::kCbit:    CASE_TRANSLATE_INSTR_(cbit);
    case Opcode::kJ:       CASE_TRANSLATE_INSTR_(j);
    case Opcode::kAddi:    CASE_TRANSLATE_INSTR_(addi);
    case Opcode::kJalr:    CASE_TRANSLATE_INSTR_(jalr);
    case Opcode::kSt:      CASE_TRANSLATE_INSTR_(st);
    case Opcode::kStp:     CASE_TRANSLATE_INSTR_(stp);
    case Opcode::kLi:      CASE_TRANSLATE_INSTR_(li);
    default:
      throw std::runtime_error("exec: unknown instr");
      break;
  }
}

#undef CASE_TRANSLATE_INSTR_

asmjit::x86::Mem JitExecutor::reg_to_dword_ptr(std::size_t reg)
{
  std::size_t reg_ptr = reinterpret_cast<std::size_t>(
                          &m_cpu->get_reg(reg));
  return asmjit::x86::dword_ptr(reg_ptr);
}

asmjit::x86::Mem JitExecutor::pc_to_dword_ptr()
{
  std::size_t pc_ptr = reinterpret_cast<std::size_t>(
                          &m_cpu->pc());
  return asmjit::x86::dword_ptr(pc_ptr);
}

asmjit::x86::Mem JitExecutor::mem_to_dword_ptr()
{
  std::size_t pc_ptr = reinterpret_cast<std::size_t>(
                          &m_cpu->mem());
  return asmjit::x86::dword_ptr(pc_ptr);
}

#define OPC_TO_INT(opc) \
  (static_cast<std::uint8_t>(opc))

void JitExecutor::translate_bdep(const Instr& inst,
                                 const JitCompilerData& cdat)
{
  cdat.cc.mov(cdat.tmp1, reg_to_dword_ptr(inst.f1));
  cdat.cc.mov(cdat.tmp2, reg_to_dword_ptr(inst.f2));
  cdat.cc.pdep(cdat.tmp3, cdat.tmp1, cdat.tmp2);
  cdat.cc.mov(reg_to_dword_ptr(inst.f3), cdat.tmp1);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_nor(const Instr& inst,
                                const JitCompilerData& cdat)
{
  cdat.cc.mov(cdat.tmp1, reg_to_dword_ptr(inst.f2));
  cdat.cc.mov(cdat.tmp2, reg_to_dword_ptr(inst.f3));
  cdat.cc.or_(cdat.tmp1, cdat.tmp2);
  cdat.cc.not_(cdat.tmp1);
  cdat.cc.mov(reg_to_dword_ptr(inst.f1), cdat.tmp1);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_cls(const Instr& inst,
                                const JitCompilerData& cdat)
{
  cdat.cc.mov(cdat.tmp1, reg_to_dword_ptr(inst.f1));
  cdat.cc.lzcnt(cdat.tmp1, cdat.tmp1);
  cdat.cc.mov(reg_to_dword_ptr(inst.f2), cdat.tmp1);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

// FIXME
void JitExecutor::translate_syscall(const Instr& inst,
                                    const JitCompilerData& cdat)
{

}

void JitExecutor::translate_add(const Instr& inst,
                                const JitCompilerData& cdat)
{
  cdat.cc.mov(cdat.tmp1, reg_to_dword_ptr(inst.f2));
  cdat.cc.mov(cdat.tmp2, reg_to_dword_ptr(inst.f3));
  cdat.cc.add(cdat.tmp1, cdat.tmp2);
  cdat.cc.mov(reg_to_dword_ptr(inst.f1), cdat.tmp1);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_ssat(const Instr& inst,
                                const JitCompilerData& cdat)
{
  cdat.cc.mov(cdat.tmp1, reg_to_dword_ptr(inst.f2));
  cdat.cc.mov(cdat.tmp2, reg_to_dword_ptr(inst.f3));

  // FIXME 

  cdat.cc.mov(reg_to_dword_ptr(inst.f1), cdat.tmp1);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

// FIXME
void JitExecutor::translate_beq(const Instr& inst,
                                const JitCompilerData& cdat)
{
  Word ofst = sgn_extend(
    inst.f3 << 2,
    kInstrEnc[OPC_TO_INT(Opcode::kBeq)].f3.width + 2);

  asmjit::Label branch_taken{};
  
  // propagated increase of PC
  cdat.cc.mov(cdat.tmp3, pc_to_dword_ptr());
  cdat.cc.add(cdat.tmp3, cdat.pc_inc);

  cdat.cc.mov(cdat.tmp1, reg_to_dword_ptr(inst.f1));
  cdat.cc.mov(cdat.tmp2, reg_to_dword_ptr(inst.f2));
  cdat.cc.cmp(cdat.tmp1, cdat.tmp2);
  cdat.cc.je(branch_taken);

  cdat.cc.add(cdat.tmp3, sizeof(Word));
  cdat.cc.mov(pc_to_dword_ptr(), cdat.tmp3);
  cdat.cc.ret();

  cdat.cc.bind(branch_taken);
  
  cdat.cc.mov(cdat.tmp1, reg_to_dword_ptr(inst.f2));
  cdat.cc.add(cdat.tmp1, ofst);
  cdat.cc.add(cdat.tmp3, cdat.tmp1);
  cdat.cc.mov(pc_to_dword_ptr(), cdat.tmp3);
  cdat.cc.ret();
}

void JitExecutor::translate_ld(const Instr& inst,
                               const JitCompilerData& cdat)
{
  Word ofst = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kSt)].f3.width);

  cdat.cc.mov(cdat.tmp1, reg_to_dword_ptr(inst.f2));
  cdat.cc.add(cdat.tmp1, ofst); // base + offst
  cdat.cc.mov(cdat.tmp2, mem_to_dword_ptr()); // mem start
  cdat.cc.add(cdat.tmp2, cdat.tmp1); // addr

  // FIXME raise exception
  
  cdat.cc.mov(cdat.tmp3, asmjit::x86::dword_ptr(cdat.tmp2));
  cdat.cc.mov(reg_to_dword_ptr(inst.f1), cdat.tmp3);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_cbit(const Instr& inst,
                                 const JitCompilerData& cdat)
{
  cdat.cc.mov(cdat.tmp1, 1 << inst.f3); // mask
  cdat.cc.mov(cdat.tmp2, inst.f2);

  cdat.cc.and_(cdat.tmp1, cdat.tmp2); // clr bit

  cdat.cc.mov(reg_to_dword_ptr(inst.f1), cdat.tmp1);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_j(const Instr& inst,
                              const JitCompilerData& cdat)
{
  // propagated increase of PC
  cdat.cc.mov(cdat.tmp1, pc_to_dword_ptr());
  cdat.cc.add(cdat.tmp1, cdat.pc_inc);

  cdat.cc.and_(cdat.tmp1, 0xf0000000);
  cdat.cc.or_(cdat.tmp1, inst.f3);

  cdat.cc.mov(pc_to_dword_ptr(), cdat.tmp1);
  cdat.cc.ret();
}

void JitExecutor::translate_addi(const Instr& inst,
                                 const JitCompilerData& cdat)
{
  Word imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kAddi)].f3.width);

  cdat.cc.mov(cdat.tmp1, reg_to_dword_ptr(inst.f2));
  cdat.cc.add(cdat.tmp1, imm);
  cdat.cc.mov(reg_to_dword_ptr(inst.f1), cdat.tmp1);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_jalr(const Instr& inst,
                                 const JitCompilerData& cdat)
{
  // propagated increase of PC
  cdat.cc.mov(cdat.tmp1, pc_to_dword_ptr());
  cdat.cc.add(cdat.tmp1, cdat.pc_inc);

  cdat.cc.mov(cdat.tmp2, cdat.tmp2);
  cdat.cc.add(cdat.tmp2, sizeof(Word)); // link
  cdat.cc.mov(pc_to_dword_ptr(), cdat.tmp2);

  Word imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kJalr)].f3.width);

  cdat.cc.mov(cdat.tmp1, reg_to_dword_ptr(inst.f2));
  cdat.cc.add(cdat.tmp1, imm);
  cdat.cc.and_(cdat.tmp1, 0xfffffffe);
  cdat.cc.mov(pc_to_dword_ptr(), cdat.tmp1);

  cdat.cc.ret();
}

void JitExecutor::translate_st(const Instr& inst,
                               const JitCompilerData& cdat)
{
  Word ofst = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kSt)].f3.width);

  cdat.cc.mov(cdat.tmp1, reg_to_dword_ptr(inst.f2));
  cdat.cc.add(cdat.tmp1, ofst); // base + offst
  cdat.cc.mov(cdat.tmp2, mem_to_dword_ptr()); // mem start
  cdat.cc.add(cdat.tmp2, cdat.tmp1); // addr

  // FIXME raise exception
  
  cdat.cc.mov(cdat.tmp3, reg_to_dword_ptr(inst.f1));
  cdat.cc.mov(asmjit::x86::dword_ptr(cdat.tmp2), cdat.tmp3);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_stp(const Instr& inst,
                                const JitCompilerData& cdat)
{
  Word ofst = sgn_extend(
    inst.f4,
    kInstrEnc[OPC_TO_INT(Opcode::kStp)].f4.width);

  cdat.cc.mov(cdat.tmp1, reg_to_dword_ptr(inst.f3));
  cdat.cc.add(cdat.tmp1, ofst); // base + offst
  cdat.cc.mov(cdat.tmp2, mem_to_dword_ptr()); // mem start
  cdat.cc.add(cdat.tmp2, cdat.tmp1); // addr
  
  // FIXME raise exception

  cdat.cc.mov(cdat.tmp3, reg_to_dword_ptr(inst.f1));
  cdat.cc.mov(asmjit::x86::dword_ptr(cdat.tmp2), cdat.tmp3);
  cdat.cc.add(cdat.tmp2, sizeof(Word));
  cdat.cc.mov(cdat.tmp3, reg_to_dword_ptr(inst.f2));
  cdat.cc.mov(asmjit::x86::dword_ptr(cdat.tmp2), cdat.tmp3);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_li(const Instr& inst,
                               const JitCompilerData& cdat)
{
  Word imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT(Opcode::kLi)].f3.width);

  cdat.cc.mov(reg_to_dword_ptr(inst.f1), imm);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

#undef OPC_TO_INT

void JitExecutor::interpret(const BasicBlk& blk)
{
  for(const auto& instr : blk.instructions)
    exec(instr);
}

Reg JitExecutor::prefetch_basic_block(BasicBlk &blk)
{
  blk.exec_cnt = 0;
  Reg pc_begin_blk = pc();
  Instr instr { .opc = Opcode::kUnknown };
  while (!IS_CONTROL_INSTRUCTION(instr)) {
    instr = decode(fetch());
    blk.instructions.push_back(instr);
    m_cpu->increment_pc(sizeof(Word));
  }
  m_cpu->set_pc(pc_begin_blk);
  return pc_begin_blk;
}
