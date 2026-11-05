#pragma once

#include <cassert>

#include "isa.hpp"
#include "memory.hpp"

struct CpuState
{
public:
  
  CpuState()
    : m_gpr {}, m_pc {0}
  {}

  void set_reg(std::size_t reg, Reg val) 
  {
    assert(reg < Isa::kInstrCnt);
    m_gpr[reg] = val; 
  }

  const Reg& get_reg(std::size_t reg) 
  { 
    assert(reg < Isa::kInstrCnt); 
    return m_gpr[reg]; 
  }

  void increment_pc(Word incr) { 
    m_pc += static_cast<Reg>(incr);
  }

  void set_pc(Reg pc_new) { m_pc = pc_new; }

  const Reg& pc() { return m_pc; }

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


