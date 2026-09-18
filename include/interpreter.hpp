#pragma once
#include "instr.hpp"
#include "cpu.hpp"

class Interpreter
{
public:
  void tick(Word &enc);

private:
  Cpu m_cpu;
};
