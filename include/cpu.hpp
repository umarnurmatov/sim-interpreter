#pragma once

#include <bit>
#include <cstdint>
#include <queue>

#include "isa.hpp"
#include "memory.hpp"
#include "instr.hpp"

#include "debug.hpp"

struct CpuState
{
public:
  
  CpuState()
    : m_gpr {}, m_pc {0}
  {}

  void set_reg(std::size_t reg, Reg val) { m_gpr[reg] = val; }

  Reg get_reg(std::size_t reg) { return m_gpr[reg]; }

  void increment_pc(SignedWord incr) { 
    m_pc += static_cast<Reg>(incr);
  }

  void set_pc(Reg pc_new) { m_pc = pc_new; }

  Reg pc() { return m_pc; }

  Memory& mem() { return m_mem; }

private:
  Reg    m_gpr[Isa::kNumRegs];
  Reg    m_pc;
  Memory m_mem;
};

struct SyscallTrap
{
  Reg num;
  std::array<Reg, Isa::kSyscallArgCnt> args;
};

#define CPU_DECLARE_EXEC_FUNC_(cmd) void exec_##cmd(BasicBlk& blk)

class Cpu
{  
public:
  Cpu();


  Word  fetch();
  Instr decode(Word enc);

  using BasicBlk = std::queue<Instr>; 
  void  exec_block(BasicBlk& blk);

  // @param blk ref to blk where instruction will be stored
  // @param max_instr_cnt maximum instruction cnt in block
  // @return pc of the beginning of the block
  Reg   prefetch_basic_block(BasicBlk& blk, 
                             std::size_t max_instr_cnt = 
                              std::numeric_limits<std::size_t>::max());

  Reg   pc() const { return m_cpu->pc(); }

  // loads binary to address 0 and resets pc
  // @param bin binary instructions
  void  load_binary(const std::vector<std::byte> &bin);

  ~Cpu();

  IF_DEBUG(
    Reg get_reg(std::size_t reg) const { 
    return m_cpu->get_reg(reg); 
  })

private:
  CPU_DECLARE_EXEC_FUNC_(unknown);
  CPU_DECLARE_EXEC_FUNC_(bdep);
  CPU_DECLARE_EXEC_FUNC_(nor);
  CPU_DECLARE_EXEC_FUNC_(cls);
  CPU_DECLARE_EXEC_FUNC_(syscall);
  CPU_DECLARE_EXEC_FUNC_(add);
  CPU_DECLARE_EXEC_FUNC_(ssat);
  CPU_DECLARE_EXEC_FUNC_(beq);
  CPU_DECLARE_EXEC_FUNC_(ld);
  CPU_DECLARE_EXEC_FUNC_(cbit);
  CPU_DECLARE_EXEC_FUNC_(j);
  CPU_DECLARE_EXEC_FUNC_(addi);
  CPU_DECLARE_EXEC_FUNC_(jalr);
  CPU_DECLARE_EXEC_FUNC_(st);
  CPU_DECLARE_EXEC_FUNC_(stp);
  CPU_DECLARE_EXEC_FUNC_(li);

  CpuState* m_cpu;

  using ExecHandler = void (Cpu::*)(BasicBlk&);
  // handlers[Opcode::k<instr_name>] = &exec_<instr_name>;
  std::array<ExecHandler, Isa::kInstrCnt + 1> m_handlers;
};

#undef CPU_DECLARE_EXEC_FUNC_
