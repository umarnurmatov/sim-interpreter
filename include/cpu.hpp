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

class Cpu
{  
public:
  Cpu();
  Word  fetch();
  Instr decode(Word enc);
  void  exec(Instr inst);
  Reg   pc() const { return m_cpu->pc(); }

  void  load_binary(std::vector<std::byte> &bin);
  ~Cpu();

  IF_DEBUG(
    Reg get_reg(std::size_t reg) const { return m_cpu->get_reg(reg); })

private:
  CpuState* m_cpu;
};
