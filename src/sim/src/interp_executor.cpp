#include "interp_executor.hpp"
#include "syscall.hpp"

int InterpExecutor::run()
{  
  bool halted = false;
  int exit_code = 0;

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
      SyscallRet ret = handle_syscall(trap);
      halted = ret.halted;
      exit_code = ret.exit_code;
    }
  }

  return exit_code;
}

Reg InterpExecutor::prefetch_basic_block(BasicBlk &blk)
{
  Reg pc_begin_blk = pc();
  Instr instr { .opc = Opcode::kUnknown };
  while (!IS_CONTROL_INSTRUCTION(instr)) {
    instr = decode(fetch());
    blk.push_back(instr);
    m_cpu->increment_pc(sizeof(Word));
  }
  m_cpu->set_pc(pc_begin_blk);
  return pc_begin_blk;
}

void InterpExecutor::exec_block(BasicBlk& blk)
{
  for(auto& instr : blk)
    exec(instr);
}
