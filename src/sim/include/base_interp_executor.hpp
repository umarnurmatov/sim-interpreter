#pragma once

#include "base_executor.hpp"

#define DECLARE_EXEC_FUNC_(cmd) void exec_##cmd(Instr inst)

class BaseInterpExecutor : public BaseExecutor
{
protected:

  void exec(Instr inst);

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
};

#undef DECLARE_EXEC_FUNC_
