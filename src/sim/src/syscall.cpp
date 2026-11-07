#include "syscall.hpp"

#include <stdexcept>

SyscallRet handle_syscall(const SyscallTrap& trap)
{
  SyscallRet ret;
  switch (trap.num) {
    case Isa::kSyscallExit:
      ret.exit_code = trap.args[0];
      ret.halted = true;
      break;
    default:
      throw std::runtime_error("unknown syscall");
  }

  return ret;
}
