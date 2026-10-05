#include "interpreter.hpp"

#include <fstream>
#include <ios>
#include <iostream>

void Interpreter::tick()
{

  try {
    Cpu::BasicBlk blk;

    if (auto cache_it = m_cache.find(m_cpu.pc());
        cache_it != m_cache.end()) {
      blk = cache_it->second;
      m_cpu.exec_block(blk);
      return;
    }
    
    Reg pc_begin_blk = m_cpu.prefetch_basic_block(blk);
    m_cache[pc_begin_blk] = blk;
    m_cpu.exec_block(blk);
  }
  catch (const SyscallTrap &trap) {
    switch (trap.num) {
      case Isa::kSyscallExit:
        m_exit_code = trap.args[0];
        m_halted = true;
        break;
      default:
        throw std::runtime_error("interpreter: unknown syscall");
    }
  }
}

int Interpreter::load_binary_file(std::string filename)
{
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cerr << "interpreter: could not open file" << std::endl;
    return 1;
  }

  const std::streamoff size = file.tellg();
  std::vector<std::byte> bytes {static_cast<std::size_t>(size)};

  file.seekg(std::ios::beg);
  if (!file.read(reinterpret_cast<char*>(bytes.data()), size)) {
    std::cerr << "interpreter: could not read file" << std::endl;
    return 1;
  }

  m_cpu.load_binary(bytes);

  m_cache.clear();

  return 0;
}
