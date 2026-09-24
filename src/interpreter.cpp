#include "interpreter.hpp"

#include <fstream>
#include <ios>
#include <iostream>


void Interpreter::tick()
{
  Word enc = m_cpu.fetch();
  auto inst_cached = m_cache.find(enc);
  if (inst_cached != m_cache.end()) {
    m_cpu.exec(inst_cached->second);
  }
  else {
    Instr inst = m_cpu.decode(enc);
    m_cache[enc] = inst;
    m_cpu.exec(inst);
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
  if(!file.read(reinterpret_cast<char*>(bytes.data()), size)) {
    std::cerr << "interpreter: could not read file" << std::endl;
    return 1;
  }

  m_cpu.load_binary(bytes);

  return 0;
}
