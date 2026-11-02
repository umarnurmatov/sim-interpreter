#include "jit_executor.hpp"
#include "asmjit/core/code_holder.h"

JitExecutor::JitExecutor(std::size_t threshold)
  : m_threshold{ threshold }
{
}

int JitExecutor::run()
{
  bool halted = false;
  int exit_code = 1;

  while (!halted) {
    auto bt_cache_it = m_bt_cache.find(pc());
    if (bt_cache_it != m_bt_cache.end()) {
      bt_cache_it->second();
      continue;
    }

    auto bb_cache_it = m_bb_cache.find(pc());

    if (bb_cache_it == m_bb_cache.end()) {
      BasicBlk blk;
      prefetch_basic_block(blk);
      m_bb_cache[pc()] = blk;
    }

    if (bb_cache_it->second.exec_cnt >= m_threshold) {
      JitBlk code = translate(bb_cache_it->second);
      m_bt_cache[pc()] = code;
      code();
    }
    else {
      bb_cache_it->second.exec_cnt++;
      interpret(bb_cache_it->second);
    }
  }

  return exit_code;
}

JitExecutor::JitBlk JitExecutor::translate(const BasicBlk& blk)
{
  asmjit::CodeHolder code; 
  code.init(m_runtime.environment(), 
            m_runtime.cpu_features());

  asmjit::x86::Compiler cc{ &code };

  cc.add_func(asmjit::FuncSignature::build<void>());

  for (const auto& instr : blk.instructions)
    translate_instr(instr, cc);

  JitBlk fn = nullptr;

  asmjit::Error cc_err = m_runtime.add(&fn, &code);
  if (cc_err != asmjit::Error::kOk) {
    // TODO
  }

  return fn;
}

#define CASE_TRANSLATE_INSTR_(cmd) \
  { translate_##cmd(inst, cc); break; }

void JitExecutor::translate_instr(const Instr& inst,
                                  const asmjit::x86::Compiler& cc)
{
  switch(inst.opc) {
    case Opcode::kBdep:    CASE_TRANSLATE_INSTR_(bdep);
    case Opcode::kNor:     CASE_TRANSLATE_INSTR_(nor);
    case Opcode::kCls:     CASE_TRANSLATE_INSTR_(cls);
    case Opcode::kSyscall: CASE_TRANSLATE_INSTR_(syscall);
    case Opcode::kAdd:     CASE_TRANSLATE_INSTR_(add);
    case Opcode::kSsat:    CASE_TRANSLATE_INSTR_(ssat);
    case Opcode::kBeq:     CASE_TRANSLATE_INSTR_(beq);
    case Opcode::kLd:      CASE_TRANSLATE_INSTR_(ld);
    case Opcode::kCbit:    CASE_TRANSLATE_INSTR_(cbit);
    case Opcode::kJ:       CASE_TRANSLATE_INSTR_(j);
    case Opcode::kAddi:    CASE_TRANSLATE_INSTR_(addi);
    case Opcode::kJalr:    CASE_TRANSLATE_INSTR_(jalr);
    case Opcode::kSt:      CASE_TRANSLATE_INSTR_(st);
    case Opcode::kStp:     CASE_TRANSLATE_INSTR_(stp);
    case Opcode::kLi:      CASE_TRANSLATE_INSTR_(li);
    default:
      throw std::runtime_error("exec: unknown instr");
      break;
  }
}

#undef CASE_TRANSLATE_INSTR_

void JitExecutor::interpret(const BasicBlk& blk)
{
  for(const auto& instr : blk.instructions)
    exec(instr);
}

Reg JitExecutor::prefetch_basic_block(BasicBlk &blk)
{
  blk.exec_cnt = 0;
  Reg pc_begin_blk = pc();
  Instr instr { .opc = Opcode::kUnknown };
  while (!IS_CONTROL_INSTRUCTION(instr)) {
    instr = decode(fetch());
    blk.instructions.push_back(instr);
    m_cpu->increment_pc(sizeof(Word));
  }
  m_cpu->set_pc(pc_begin_blk);
  return pc_begin_blk;
}


