#include "jit_executor.hpp"

#include <iostream>
#include <stdexcept>

#include "asmjit/core/code_holder.h"
#include "asmjit/x86/x86_operand.h"

#include "bitops.hpp"
#include "cpu_state.hpp"
#include "syscall.hpp"


JitExecutor::JitExecutor(std::size_t threshold)
  : m_threshold{ threshold },
    m_log_stream{ LOG_FILE },
    m_logger{ m_log_stream }
{
}

JitExecutor::JitCompilerData::JitCompilerData(asmjit::x86::Compiler& cc_)
  : cc{ cc_ }
{
}

void JitExecutor::JitCompilerData::init(CpuState* cpu)
{
  tmp1 = cc.new_gp32();
  tmp2 = cc.new_gp32();
  tmp3 = cc.new_gp32();

  pc_inc = cc.new_gp32();

  ram_base_addr      = cc.new_gp_ptr();
  pc_addr       = cc.new_gp_ptr();
  reg_file_addr = cc.new_gp_ptr();

  asmjit::Imm reg_file_addr_imm = 
    asmjit::imm(reinterpret_cast<std::size_t>(&cpu->get_reg(0)));

  asmjit::Imm pc_addr_imm = 
    asmjit::imm(reinterpret_cast<std::size_t>(&cpu->pc()));

  asmjit::Imm ram_addr_imm = 
    asmjit::imm(reinterpret_cast<std::size_t>(cpu->mem().get_data()));

  cc.mov(pc_inc, 0);

  cc.mov(ram_base_addr,      ram_addr_imm);
  cc.mov(pc_addr,       pc_addr_imm);
  cc.mov(reg_file_addr, reg_file_addr_imm);
}

int JitExecutor::run()
{
  bool halted = false;
  int exit_code = 0;

  while (!halted) {
    try {
      auto bt_cache_it = m_bt_cache.find(pc());
      if (bt_cache_it != m_bt_cache.end()) {
        handle_blk_on_exit(bt_cache_it->second());
        continue;
      }

      auto bb_cache_it = m_bb_cache.find(pc());

      if (bb_cache_it == m_bb_cache.end()) {
        BasicBlk blk;
        prefetch_basic_block(blk);
        bb_cache_it = m_bb_cache.insert({ pc(), blk }).first;
      }

      if (bb_cache_it->second.exec_cnt >= m_threshold) {
        JitBlk blk = translate(bb_cache_it->second);
        m_bt_cache[pc()] = blk;
        handle_blk_on_exit(blk());
      }
      else {
        bb_cache_it->second.exec_cnt++;
        interpret(bb_cache_it->second);
      }
    }
    catch (const SyscallTrap& trap) {
      SyscallRet ret = handle_syscall(trap);
      exit_code = ret.exit_code;
      halted = ret.halted;
    }
  }

  return exit_code;
}

void JitExecutor::handle_blk_on_exit(BlkExitCode blk_exit_code)
{
  switch (blk_exit_code) {
    case kExitSuccess: break;
    case kSyscall: {
      SyscallTrap trap {};
      trap.num = m_cpu->get_reg(Isa::x8);
      for (std::size_t i = 0; i < Isa::kSyscallArgCnt; ++i)
        trap.args[i] = m_cpu->get_reg(i);
      throw trap;
    }
    case kMemoryMisalignedAddr:
      throw std::runtime_error("memory adress misaligned");

    default: break;
  }
}

JitExecutor::JitBlk JitExecutor::translate(const BasicBlk& blk)
{
  std::cout << "translating block" << std::endl;
  asmjit::CodeHolder code; 
  code.init(m_runtime.environment(), 
            m_runtime.cpu_features());
  code.set_logger(&m_logger);
  code.set_error_handler(&m_err_handler);

  asmjit::x86::Compiler cc{ &code };

  JitCompilerData compiler_dat{ cc };

  cc.add_func(asmjit::FuncSignature::build<BlkExitCode>());
  compiler_dat.init(m_cpu);

  for (const auto& instr : blk.instructions)
    translate_instr(instr, compiler_dat);

  cc.end_func();
  cc.finalize();

  JitBlk fn = nullptr;

  asmjit::Error runtime_err = m_runtime.add(&fn, &code);
  if (runtime_err != asmjit::Error::kOk) {
    throw std::runtime_error(
      asmjit::stringify_error(runtime_err));
  }

  return fn;
}

#define CASE_TRANSLATE_INSTR_(cmd)                    \
  {                                                   \
    translate_##cmd(inst, cdat);                      \
    break;                                            \
  }

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

#define OPC_TO_INT_(opc) \
  (static_cast<std::uint8_t>(opc))

#define REG_TO_DWORD_PTR_(reg) \
  asmjit::x86::dword_ptr(cdat.reg_file_addr, reg * sizeof(Reg))

#define PC_TO_DWORD_PTR_ \
  asmjit::x86::dword_ptr(cdat.pc_addr)

#define RAM_ADDR_TO_DWORD_PTR_(addr) \
  asmjit::x86::dword_ptr(cdat.ram_base_addr, addr.r64())

#define RAM_ADDR_OFFS_TO_DWORD_PTR_(addr, offst) \
  asmjit::x86::dword_ptr(cdat.ram_base_addr, addr.r64(), 0, offst)

void JitExecutor::translate_bdep(const Instr& inst,
                                 const JitCompilerData& cdat)
{
  cdat.cc.mov(cdat.tmp1, REG_TO_DWORD_PTR_(inst.f1));
  cdat.cc.mov(cdat.tmp2, REG_TO_DWORD_PTR_(inst.f2));
  cdat.cc.pdep(cdat.tmp3, cdat.tmp2, cdat.tmp1);
  cdat.cc.mov(REG_TO_DWORD_PTR_(inst.f3), cdat.tmp3);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_nor(const Instr& inst,
                                const JitCompilerData& cdat)
{
  cdat.cc.mov(cdat.tmp1, REG_TO_DWORD_PTR_(inst.f2));
  cdat.cc.mov(cdat.tmp2, REG_TO_DWORD_PTR_(inst.f3));
  cdat.cc.or_(cdat.tmp1, cdat.tmp2);
  cdat.cc.not_(cdat.tmp1);
  cdat.cc.mov(REG_TO_DWORD_PTR_(inst.f1), cdat.tmp1);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_cls(const Instr& inst,
                                const JitCompilerData& cdat)
{
  cdat.cc.mov(cdat.tmp1, REG_TO_DWORD_PTR_(inst.f1));
  cdat.cc.not_(cdat.tmp1);
  cdat.cc.lzcnt(cdat.tmp1, cdat.tmp1);
  cdat.cc.mov(REG_TO_DWORD_PTR_(inst.f2), cdat.tmp1);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_syscall(const Instr& inst,
                                    const JitCompilerData& cdat)
{
  cdat.cc.mov(cdat.tmp3, PC_TO_DWORD_PTR_);
  cdat.cc.add(cdat.tmp3, cdat.pc_inc);
  cdat.cc.add(cdat.tmp3, sizeof(Word));
  cdat.cc.mov(PC_TO_DWORD_PTR_, cdat.tmp3);

  cdat.cc.mov(cdat.tmp1, kSyscall); 
  cdat.cc.ret(cdat.tmp1);
}

void JitExecutor::translate_add(const Instr& inst,
                                const JitCompilerData& cdat)
{
  cdat.cc.mov(cdat.tmp1, REG_TO_DWORD_PTR_(inst.f2));
  cdat.cc.mov(cdat.tmp2, REG_TO_DWORD_PTR_(inst.f3));
  cdat.cc.add(cdat.tmp1, cdat.tmp2);
  cdat.cc.mov(REG_TO_DWORD_PTR_(inst.f1), cdat.tmp1);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_ssat(const Instr& inst,
                                const JitCompilerData& cdat)
{
  // if (inst.f3 == 0 || inst.f3 >= Isa::kWordWidth)
  //   throw std::runtime_error("translate_ssat: invalid saturation width");

  const auto magnitude = std::int32_t{1} << (inst.f3 - 1);
  const auto lower = -magnitude;
  const auto upper = magnitude - 1;

  // just dissasembled sng_satr()
  cdat.cc.mov(cdat.tmp1, REG_TO_DWORD_PTR_(inst.f1));
  cdat.cc.mov(cdat.tmp2, lower);
  cdat.cc.cmp(cdat.tmp1, cdat.tmp2);
  cdat.cc.cmovl(cdat.tmp1, cdat.tmp2);
  cdat.cc.mov(cdat.tmp3, upper);
  cdat.cc.cmp(cdat.tmp1, cdat.tmp3);
  cdat.cc.cmovg(cdat.tmp1, cdat.tmp3);
  cdat.cc.mov(REG_TO_DWORD_PTR_(inst.f2), cdat.tmp1);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_beq(const Instr& inst,
                                const JitCompilerData& cdat)
{
  Word ofst = sgn_extend(
    inst.f3 << 2,
    kInstrEnc[OPC_TO_INT_(Opcode::kBeq)].f3.width + 2);

  asmjit::Label branch_taken = cdat.cc.new_label();
  
  // propagated increase of PC
  cdat.cc.mov(cdat.tmp3, PC_TO_DWORD_PTR_);
  cdat.cc.add(cdat.tmp3, cdat.pc_inc);

  cdat.cc.mov(cdat.tmp1, REG_TO_DWORD_PTR_(inst.f1));
  cdat.cc.mov(cdat.tmp2, REG_TO_DWORD_PTR_(inst.f2));
  cdat.cc.cmp(cdat.tmp1, cdat.tmp2);
  cdat.cc.je(branch_taken);

  cdat.cc.add(cdat.tmp3, sizeof(Word));
  cdat.cc.mov(PC_TO_DWORD_PTR_, cdat.tmp3);
  cdat.cc.mov(cdat.tmp1, kExitSuccess);
  cdat.cc.ret(cdat.tmp1);

  cdat.cc.bind(branch_taken);
  
  cdat.cc.add(cdat.tmp3, ofst);
  cdat.cc.mov(PC_TO_DWORD_PTR_, cdat.tmp3);
  cdat.cc.mov(cdat.tmp1, kExitSuccess);
  cdat.cc.ret(cdat.tmp1);
}

void JitExecutor::translate_ld(const Instr& inst,
                               const JitCompilerData& cdat)
{
  Word ofst = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT_(Opcode::kSt)].f3.width);

  cdat.cc.mov(cdat.tmp1, REG_TO_DWORD_PTR_(inst.f2));
  cdat.cc.add(cdat.tmp1, ofst); // base + offst

  // check alignment
  asmjit::Label aligned = cdat.cc.new_label();
  cdat.cc.mov(cdat.tmp2, cdat.tmp1);
  cdat.cc.and_(cdat.tmp2, static_cast<Word>((1 << 2) - 1));
  cdat.cc.cmp(cdat.tmp2, 0);
  cdat.cc.je(aligned);
  cdat.cc.mov(cdat.tmp1, kMemoryMisalignedAddr);
  cdat.cc.ret(cdat.tmp1);
  
  cdat.cc.bind(aligned);
  cdat.cc.mov(cdat.tmp3, RAM_ADDR_TO_DWORD_PTR_(cdat.tmp1));
  cdat.cc.mov(REG_TO_DWORD_PTR_(inst.f1), cdat.tmp3);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_cbit(const Instr& inst,
                                 const JitCompilerData& cdat)
{
  cdat.cc.mov(cdat.tmp1, REG_TO_DWORD_PTR_(inst.f2));
  cdat.cc.and_(cdat.tmp1, ~(Word{1} << inst.f3)); // clr bit
  cdat.cc.mov(REG_TO_DWORD_PTR_(inst.f1), cdat.tmp1);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_j(const Instr& inst,
                              const JitCompilerData& cdat)
{
  // propagated increase of PC
  cdat.cc.mov(cdat.tmp1, PC_TO_DWORD_PTR_);
  cdat.cc.add(cdat.tmp1, cdat.pc_inc);

  cdat.cc.and_(cdat.tmp1, 0xf0000000);
  cdat.cc.or_(cdat.tmp1, inst.f3 << 2);

  cdat.cc.mov(PC_TO_DWORD_PTR_, cdat.tmp1);
  cdat.cc.mov(cdat.tmp1, kExitSuccess);
  cdat.cc.ret(cdat.tmp1);
}

void JitExecutor::translate_addi(const Instr& inst,
                                 const JitCompilerData& cdat)
{
  Word imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT_(Opcode::kAddi)].f3.width);

  cdat.cc.mov(cdat.tmp1, REG_TO_DWORD_PTR_(inst.f2));
  cdat.cc.add(cdat.tmp1, imm);
  cdat.cc.mov(REG_TO_DWORD_PTR_(inst.f1), cdat.tmp1);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_jalr(const Instr& inst,
                                 const JitCompilerData& cdat)
{
  // propagated increase of PC
  cdat.cc.mov(cdat.tmp1, PC_TO_DWORD_PTR_);
  cdat.cc.add(cdat.tmp1, cdat.pc_inc);

  cdat.cc.mov(cdat.tmp2, cdat.tmp1);
  cdat.cc.add(cdat.tmp2, sizeof(Word)); // link

  Word imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT_(Opcode::kJalr)].f3.width);

  cdat.cc.mov(cdat.tmp1, REG_TO_DWORD_PTR_(inst.f2));
  cdat.cc.add(cdat.tmp1, imm);
  cdat.cc.and_(cdat.tmp1, 0xfffffffe);
  cdat.cc.mov(PC_TO_DWORD_PTR_, cdat.tmp1);
  cdat.cc.mov(REG_TO_DWORD_PTR_(inst.f1), cdat.tmp2);

  cdat.cc.mov(cdat.tmp1, kExitSuccess);
  cdat.cc.ret(cdat.tmp1);
}

void JitExecutor::translate_st(const Instr& inst,
                               const JitCompilerData& cdat)
{
  Word ofst = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT_(Opcode::kSt)].f3.width);

  cdat.cc.mov(cdat.tmp1, REG_TO_DWORD_PTR_(inst.f2));
  cdat.cc.add(cdat.tmp1, ofst); // base + offst

  // check alignment
  asmjit::Label aligned = cdat.cc.new_label();
  cdat.cc.mov(cdat.tmp2, cdat.tmp1);
  cdat.cc.and_(cdat.tmp2, (1 << 2) - 1); 
  cdat.cc.cmp(cdat.tmp2, 0);
  cdat.cc.je(aligned);
  cdat.cc.mov(cdat.tmp1, kMemoryMisalignedAddr);
  cdat.cc.ret(cdat.tmp1);
  
  cdat.cc.bind(aligned);
  cdat.cc.mov(cdat.tmp2, REG_TO_DWORD_PTR_(inst.f1));
  cdat.cc.mov(RAM_ADDR_TO_DWORD_PTR_(cdat.tmp1), cdat.tmp2);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_stp(const Instr& inst,
                                const JitCompilerData& cdat)
{
  Word ofst = sgn_extend(
    inst.f4,
    kInstrEnc[OPC_TO_INT_(Opcode::kStp)].f4.width);

  cdat.cc.mov(cdat.tmp1, REG_TO_DWORD_PTR_(inst.f3));
  cdat.cc.add(cdat.tmp1, ofst); // base + offst
  
  // check alignment
  asmjit::Label aligned = cdat.cc.new_label();
  cdat.cc.mov(cdat.tmp2, cdat.tmp1);
  cdat.cc.and_(cdat.tmp2, static_cast<Word>((1 << 2) - 1)); 
  cdat.cc.cmp(cdat.tmp2, 0);
  cdat.cc.je(aligned);
  cdat.cc.mov(cdat.tmp1, kMemoryMisalignedAddr);
  cdat.cc.ret(cdat.tmp1);

  cdat.cc.bind(aligned);
  cdat.cc.mov(cdat.tmp3, REG_TO_DWORD_PTR_(inst.f1));
  cdat.cc.mov(RAM_ADDR_TO_DWORD_PTR_(cdat.tmp1), cdat.tmp3);
  cdat.cc.mov(cdat.tmp3, REG_TO_DWORD_PTR_(inst.f2));
  cdat.cc.mov(RAM_ADDR_OFFS_TO_DWORD_PTR_(cdat.tmp1, sizeof(Word)), cdat.tmp3);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

void JitExecutor::translate_li(const Instr& inst,
                               const JitCompilerData& cdat)
{
  Word imm = sgn_extend(
    inst.f3,
    kInstrEnc[OPC_TO_INT_(Opcode::kLi)].f3.width);

  cdat.cc.mov(REG_TO_DWORD_PTR_(inst.f1), imm);

  cdat.cc.add(cdat.pc_inc, sizeof(Word));
}

#undef OPC_TO_INT_
#undef REG_TO_DWORD_PTR_
#undef PC_TO_DWORD_PTR_
#undef RAM_ADDR_TO_DWORD_PTR_
#undef RAM_ADDR_OFFS_TO_DWORD_PTR_

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
