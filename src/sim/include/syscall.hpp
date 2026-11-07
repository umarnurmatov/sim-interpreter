#pragma once

#include <array>

#include "isa.hpp"

struct SyscallTrap
{
  Reg num;
  std::array<Reg, Isa::kSyscallArgCnt> args;
};

struct SyscallRet
{
  bool halted;
  int exit_code;
};

SyscallRet handle_syscall(const SyscallTrap& trap);
