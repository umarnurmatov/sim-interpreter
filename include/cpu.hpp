#pragma once
#include <vector>
#include <cstdint>

#include "instr.hpp"

using Reg = std::uint32_t;

constexpr std::size_t kNumRegs = 32;

struct Memory
{
  std::vector<std::uint8_t> ram;
};

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
