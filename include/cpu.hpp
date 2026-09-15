#pragma once
#include <cstdint>
#include <vector>

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

