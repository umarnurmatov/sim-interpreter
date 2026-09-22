#pragma once

#include <cstring>
#include <stdexcept>
#include <vector>
#include <cstdint>

#include "isa.hpp"

class Memory
{
public:
  Memory()
    : Memory(kRamSize)
  {
  }

  Memory(const std::size_t ram_sz) 
  {
    m_ram.resize(ram_sz);
  }

  template<MemoryType T>
  T load(std::size_t addr) const;

  template<MemoryType T>
  void store(std::size_t addr, T value);

private:
  template <MemoryType T>
  void check_bounds(std::size_t addr) const;

  std::vector<std::uint8_t> m_ram;
};


template <MemoryType T>
T Memory::load(std::size_t addr) const
{
  check_bounds<T>(addr);

  T value{};
  std::memcpy(&value, m_ram.data() + addr, sizeof(T));

  return value;
}

template <MemoryType T>
void Memory::store(std::size_t addr, T value)
{
  check_bounds<T>(addr);
  std::memcpy(m_ram.data() + addr, &value, sizeof(T));
}

template <MemoryType T>
void Memory::check_bounds(std::size_t addr) const
{
  if (addr > m_ram.size() || sizeof(T) > m_ram.size() - addr)
    throw std::runtime_error("memory: access out of bounds");
}
