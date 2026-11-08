#include "syscall.hpp"

#include <stdexcept>
#include <syscall.h>
#include <unistd.h>

#include "isa.hpp"

#ifdef __linux__

SyscallRet handle_syscall(const SyscallTrap& trap)
{
  SyscallRet ret { false, 0 };

  switch (trap.num) {
    case Isa::kSyscallRead:
      read(trap.args[0], 
           reinterpret_cast<void*>(trap.args[1]),
           trap.args[2]);
      break;
    case Isa::kSyscallWrite:
      write(trap.args[0], 
            reinterpret_cast<const void*>(trap.args[1]),
            trap.args[2]);
      break;
    case Isa::kSyscallExit:
      ret.exit_code = trap.args[0];
      ret.halted = true;
      break;
    
    default:
      throw std::runtime_error("unknown syscall");
  }

  return ret;
}

#else

#error "unsopperted host platform"

#endif
