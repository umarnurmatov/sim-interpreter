#pragma once
#include <unordered_map>
#include "instr.hpp"
#include "cpu.hpp"

class Interpreter
{
public:
  void tick(Word &enc);

private:

  CpuState                        cpu;
  std::unordered_map<Word, Instr> cache;

  Instr decode(Word &enc);
  void  exec(Instr &inst);
};
