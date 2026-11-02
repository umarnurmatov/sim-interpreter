#pragma once

#include <unordered_map>

#include "base_interp_executor.hpp"

class InterpExecutor : public BaseInterpExecutor
{
public:
  enum Mode {
    kLinear,
    kCached
  };

  InterpExecutor(Mode mode = kCached)
    : m_mode{ mode }
  {}

  int run() override;

private:

  using BasicBlk = struct std::vector<Instr>;

  // @param blk ref to blk where instruction will be stored
  // @param max_instr_cnt maximum instruction cnt in block
  // @return pc of the beginning of the block
  Reg prefetch_basic_block(BasicBlk& blk);

  void exec_block(BasicBlk& blk);

  using BlkCache = std::unordered_map<Reg, BasicBlk>;
  BlkCache m_cache;

  Mode m_mode;
};


