#pragma once

#include "isa.hpp"
#include "instr.hpp"
#include "cpu_state.hpp"

class BaseExecutor
{  
public:
  BaseExecutor();

  virtual int run() = 0;

  void load_bytes(std::size_t addr, const char* bytes, 
                  std::size_t size);

  virtual ~BaseExecutor();

protected:
  CpuState* m_cpu;

  Reg pc() const { return m_cpu->pc(); }

  Word fetch();
  Instr decode(Word enc);
};
