#pragma once

#include <unordered_map>
#include <fstream>
#include <vector>

#include <asmjit/x86.h>

#include "base_interp_executor.hpp"

constexpr std::size_t kThresholdDefault = 0;

class AsmJitException : public std::exception {
public:
  asmjit::Error err;
  std::string message;
 
  AsmJitException(asmjit::Error err, const char* message) noexcept
    : err(err),
      message(message) {}
 
  const char* what() const noexcept override { 
    return message.c_str(); 
  }
};
 
class AsmjitErrorHandler : public asmjit::ErrorHandler {
public:
  void handle_error(asmjit::Error err, 
                    const char* message, 
                    asmjit::BaseEmitter* origin) override {
    throw AsmJitException(err, message);
  }
};

class JitExecutor : public BaseInterpExecutor
{
public:
    
  JitExecutor(std::size_t threshold = kThresholdDefault);

  int run() override;

private:
  enum BlkExitCode : int {
    kExitSuccess,
    kSyscall,
    kMemoryMisalignedAddr
  };

  using JitBlk = BlkExitCode (*)(void);

  struct JitCompilerData {
    JitCompilerData(asmjit::x86::Compiler& cc_);

    // call after function attached to cc
    void init(CpuState* cpu);

    asmjit::x86::Compiler& cc;

    asmjit::x86::Gp tmp1;
    asmjit::x86::Gp tmp2;
    asmjit::x86::Gp tmp3;

    // for PC folding optimization
    asmjit::x86::Gp pc_inc; 

    asmjit::x86::Gp ram_base_addr;
    asmjit::x86::Gp pc_addr;
    asmjit::x86::Gp reg_file_addr;
  };

  struct BasicBlk {
    std::vector<Instr> instructions;
    std::size_t exec_cnt;
  };

  void handle_blk_on_exit(BlkExitCode blk_exit_code);

  JitBlk translate(const BasicBlk& blk);

  void translate_instr(const Instr& inst,
                       const JitCompilerData& cdat);

#define DECLARE_TRANSLATE_FUNC_(cmd)                \
  void translate_##cmd(const Instr& inst,           \
                       const JitCompilerData& cdat)

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

  AsmjitErrorHandler m_err_handler;

  std::unordered_map<Reg, JitBlk>   m_bt_cache;
  std::unordered_map<Reg, BasicBlk> m_bb_cache;

  std::size_t m_threshold;
};
