#pragma once

#include "isa.hpp"
#include "instr.hpp"
#include "cpu_state.hpp"

class BaseExecutor
{  
public:
  BaseExecutor();

  virtual int run() = 0;

  // loads binary to address 0 and resets pc
  // @param bin binary instructions
  void load_binary(const std::vector<std::byte> &bin);

  virtual ~BaseExecutor();

protected:
  CpuState* m_cpu;

  Reg pc() const { return m_cpu->pc(); }

  Word fetch();
  Instr decode(Word enc);
};
