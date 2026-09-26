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
  using BlkCache = std::unordered_map<Reg, Cpu::BasicBlk>;
  BlkCache m_cache;
  bool     m_halted;
  int      m_exit_code;
  Cpu      m_cpu;
};
