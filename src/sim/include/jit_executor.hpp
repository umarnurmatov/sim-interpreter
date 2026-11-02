#pragma once

#include <asmjit/x86.h>

#include <unordered_map>

#include "base_interp_executor.hpp"

class JitExecutor : public BaseInterpExecutor
{
public:
    
  JitExecutor();

  int run() override;

private:
  void translate();

  asmjit::JitRuntime m_runtime;
};
