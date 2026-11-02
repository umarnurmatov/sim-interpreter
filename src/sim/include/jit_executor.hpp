#pragma once

#include <unordered_map>

#include <asmjit/x86.h>

#include "base_interp_executor.hpp"

constexpr std::size_t kThresholdDefault = 10;

class JitExecutor : public BaseInterpExecutor
{
public:
    
  JitExecutor(std::size_t threshold = kThresholdDefault);

  int run() override;

private:
  using JitBlk = void (*)(void);

  struct BasicBlk {
    std::vector<Instr> instructions;
    std::size_t exec_cnt;
  };

  JitBlk translate(const BasicBlk& blk);

  void translate_instr(const Instr& inst,
                       const asmjit::x86::Compiler& cc);

#define DECLARE_TRANSLATE_FUNC_(cmd)                    \
  void translate_##cmd(Instr inst,                      \
                       const asmjit::x86::Compiler& cc)

  DECLARE_TRANSLATE_FUNC_(bdep);
  DECLARE_TRANSLATE_FUNC_(nor);
  DECLARE_TRANSLATE_FUNC_(cls);
  DECLARE_TRANSLATE_FUNC_(syscall);
  DECLARE_TRANSLATE_FUNC_(add);
  DECLARE_TRANSLATE_FUNC_(ssat);
  DECLARE_TRANSLATE_FUNC_(beq);
  DECLARE_TRANSLATE_FUNC_(ld);
  DECLARE_TRANSLATE_FUNC_(cbit);
  DECLARE_TRANSLATE_FUNC_(j);
  DECLARE_TRANSLATE_FUNC_(addi);
  DECLARE_TRANSLATE_FUNC_(jalr);
  DECLARE_TRANSLATE_FUNC_(st);
  DECLARE_TRANSLATE_FUNC_(stp);
  DECLARE_TRANSLATE_FUNC_(li);

#undef DECLARE_TRANSLATE_FUNC_

  void interpret(const BasicBlk& blk);

  // @param blk ref to blk where instruction will be stored
  // @param max_instr_cnt maximum instruction cnt in block
  // @return pc of the beginning of the block
  Reg prefetch_basic_block(BasicBlk& blk);

  asmjit::JitRuntime m_runtime;

  std::unordered_map<Reg, JitBlk>   m_bt_cache;
  std::unordered_map<Reg, BasicBlk> m_bb_cache;

  std::size_t m_threshold;
};
