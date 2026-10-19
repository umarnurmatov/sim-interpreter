#include "base_executor.hpp"
#include <unordered_map>

#define DECLARE_EXEC_FUNC_(cmd) void exec_##cmd(Instr inst)

class InterpExecutor : public BaseExecutor
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

  void exec(Instr inst);

  DECLARE_EXEC_FUNC_(bdep);
  DECLARE_EXEC_FUNC_(nor);
  DECLARE_EXEC_FUNC_(cls);
  DECLARE_EXEC_FUNC_(syscall);
  DECLARE_EXEC_FUNC_(add);
  DECLARE_EXEC_FUNC_(ssat);
  DECLARE_EXEC_FUNC_(beq);
  DECLARE_EXEC_FUNC_(ld);
  DECLARE_EXEC_FUNC_(cbit);
  DECLARE_EXEC_FUNC_(j);
  DECLARE_EXEC_FUNC_(addi);
  DECLARE_EXEC_FUNC_(jalr);
  DECLARE_EXEC_FUNC_(st);
  DECLARE_EXEC_FUNC_(stp);
  DECLARE_EXEC_FUNC_(li);

  using BlkCache = std::unordered_map<Reg, BasicBlk>;
  BlkCache m_cache;
  Mode m_mode;
};

#undef DECLARE_EXEC_FUNC_


