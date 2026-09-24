#pragma once

#include <unordered_map>

#include "instr.hpp"
#include "cpu.hpp"

class Interpreter
{
public:
  Interpreter()
    : m_halted {false},
      m_exit_code {0}
  {}

  void tick();
  int load_binary_file(std::string filename);

  bool halted() { return m_halted; }

private:
  Cpu                             m_cpu;
  std::unordered_map<Word, Instr> m_cache;
  bool                            m_halted;
  int                             m_exit_code;
};
