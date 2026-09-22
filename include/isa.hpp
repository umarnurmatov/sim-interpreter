#pragma once

#include <cstdint>
#include <concepts>

using Word = std::uint32_t;
using Byte = std::uint8_t;
using Reg  = std::uint32_t;

template <typename T>
concept MemoryType =
  std::same_as<T, Byte> || std::same_as<T, Word>;

constexpr std::size_t kNumRegs = 32;
constexpr std::size_t kRamSize = 1024;
