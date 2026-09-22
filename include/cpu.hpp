#pragma once

#include <cstdint>

#include "isa.hpp"
#include "memory.hpp"
#include "instr.hpp"

struct CpuState
{
  Reg    gpr[kNumRegs];
  Reg    pc;
  Memory mem;

  void set_reg(std::size_t reg, Reg val)
  {
    gpr[reg] = val;
  }
  Reg get_reg(std::size_t reg)
  {
    return gpr[reg]; 
  }
};

class Cpu
{  
public:
  Cpu();
  Instr decode(Word &enc);
  void  exec  (Instr &inst);
  ~Cpu();

private:
  CpuState* m_cpu;
};
