#include "interp_executor.hpp"

int InterpExecutor::run()
{  
  bool halted = false;
  int exit_code = 1;

  while (!halted) {
    try {
      if (m_mode == kLinear) {
        exec(decode(fetch()));
      }
      else if (m_mode == kCached) {
        BasicBlk blk;

        if (auto cache_it = m_cache.find(pc());
            cache_it != m_cache.end()) {
          blk = cache_it->second;
          exec_block(blk);
          continue;
        }
        
        Reg pc_begin_blk = prefetch_basic_block(blk);
        m_cache[pc_begin_blk] = blk;
        exec_block(blk);
      }
    }
    catch (const SyscallTrap &trap) {
      switch (trap.num) {
        case Isa::kSyscallExit:
          exit_code = trap.args[0];
          halted = true;
          break;
        default:
          throw std::runtime_error("interpreter: unknown syscall");
      }
    }
  }

  return exit_code;
}
