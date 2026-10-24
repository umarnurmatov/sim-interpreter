#pragma once

#include <asmjit/x86.h>

#include "base_executor.hpp"
#include "isa.hpp"

#define DECLARE_EXEC_FUNC_(cmd) void exec_##cmd(Instr inst)

class JitExecutor : public BaseExecutor
{
public:
  JitExecutor();

  int run() override;

private:
  
  DECLARE_EXEC_FUNC_(bdep);
  DECLARE_EXEC_FUNC_(nor);
  DECLARE_EXEC_FUNC_(cls);
  DECLARE_EXEC_FUNC_(syscall);
  DECLARE_EXEC_FUNC_(add);
  DECLARE_EXEC_FUNC_(ssat);
  DECLARE_EXEC_FUNC_(beq);
  DECLARE_EXEC_FUNC_(ld);
  DECLARE_EXEC_FUNC_(cbit);
  DECLARE_EXEC_FUNC_(j);
  DECLARE_EXEC_FUNC_(addi);
  DECLARE_EXEC_FUNC_(jalr);
  DECLARE_EXEC_FUNC_(st);
  DECLARE_EXEC_FUNC_(stp);
  DECLARE_EXEC_FUNC_(li);

  asmjit::JitRuntime m_runtime;
  asmjit::CodeHolder m_code_holder;

  asmjit::x86::Compiler m_cc;
  
  // std::array<asmjit::x86::Gp, Isa::kNumRegs> m_regs;
};

#undef DECLARE_EXEC_FUNC_
