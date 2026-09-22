#include "interpreter.hpp"


void Interpreter::tick(Word &enc)
{
  auto inst_cached = m_cache.find(enc);
  if (inst_cached != m_cache.end()) {
    m_cpu.exec(inst_cached->second);
  }
  else {
    Instr inst = m_cpu.decode(enc);
    m_cache[enc] = inst;
    m_cpu.exec(inst);
  }
}
