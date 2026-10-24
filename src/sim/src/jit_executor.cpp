#include "jit_executor.hpp"
#include "asmjit/core/builder.h"
#include "isa.hpp"

JitExecutor::JitExecutor()
  : m_cc{ &m_code_holder }
{
  m_code_holder.init(m_runtime.environment(),
                     m_runtime.cpu_features());
  
  // for (size_t i = 0; i < m_regs.size(); ++i)
  //   m_regs[i] = m_cc.new_gp32();
}
