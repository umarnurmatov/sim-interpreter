#pragma once
#include <cstdint>

using Word = std::uint32_t;

enum class Opcode : std::uint8_t
{
};

struct Instr
{
  Opcode opc{};
  Word src1{}, src2{}, dst{};
};
