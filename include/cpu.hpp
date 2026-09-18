#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>

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
};

class Cpu
{  
public:
  Cpu();
  Instr decode(Word &enc);
  void  exec  (Instr &inst);

private:
  CpuState                        m_cpu;
  std::unordered_map<Word, Instr> m_cache;
};
