#pragma once

#include <unordered_map>

#include "instr.hpp"
#include "cpu.hpp"

class Interpreter
{
public:
  void tick(Word &enc);

private:
  Cpu                             m_cpu;
  std::unordered_map<Word, Instr> m_cache;
};
