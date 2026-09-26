#pragma once

#include <bit>
#include <cstdint>

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

#define CPU_DECLARE_EXEC_FUNC_(cmd) void exec_##cmd(Instr inst)

class Cpu
{  
public:
  Cpu();


  Word  fetch();
  Instr decode(Word enc);
  void  exec(Instr inst);

  using BasicBlk = std::vector<Instr>; 
  void  exec_block(BasicBlk blk);

  // @param blk ref to blk where instruction will be stored
  // @return pc of the beginning of the block
  Reg   prefetch_basic_block(BasicBlk &blk);

  Reg   pc() const { return m_cpu->pc(); }

  void  load_binary(std::vector<std::byte> &bin);
  ~Cpu();

  IF_DEBUG(
    Reg get_reg(std::size_t reg) const { 
    return m_cpu->get_reg(reg); 
  })

private:
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
};

#undef CPU_DECLARE_EXEC_FUNC_
