#pragma once

#include <cstring>
#include <stdexcept>
#include <array>
#include <cstdint>
#include <vector>

#include "isa.hpp"

class Memory
{
public:

  template<MemoryType T>
  T load(std::size_t addr) const;

  template<MemoryType T>
  void store(std::size_t addr, T value);

  void load_bytes(std::size_t addr, std::vector<std::byte> bytes)
  {
    if (addr + bytes.size() > m_ram.size() - 1)
      throw std::runtime_error("memory: access out of bounds");

    std::memcpy(m_ram.data() + addr, bytes.data(), sizeof(std::byte) * bytes.size());
  }

private:
  template <MemoryType T>
  void check_bounds(std::size_t addr) const 
  {
    if (addr >= m_ram.size() || sizeof(T) > m_ram.size() - addr)
      throw std::runtime_error("memory: access out of bounds");
  }

  void check_alignment(std::size_t addr) const 
  {
    if (addr % sizeof(Word) != 0)
      throw std::runtime_error("memory: misaligned access");
  }

  std::array<std::uint8_t, Isa::kRamSize> m_ram;
};


template <MemoryType T>
T Memory::load(std::size_t addr) const
{
  check_alignment(addr);
  check_bounds<T>(addr);

  T value{};
  std::memcpy(&value, m_ram.data() + addr, sizeof(T));

  return value;
}

template <MemoryType T>
void Memory::store(std::size_t addr, T value)
{
  check_alignment(addr);
  check_bounds<T>(addr);

  std::memcpy(m_ram.data() + addr, &value, sizeof(T));
}

