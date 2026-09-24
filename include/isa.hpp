#pragma once

#include <cstdint>
#include <concepts>

using Word       = std::uint32_t;
using SignedWord = std::int32_t;
using Byte       = std::uint8_t;
using Reg        = std::uint32_t;

constexpr std::size_t kRegWidth  = sizeof(Reg) * 8;

template <typename T>
concept MemoryType =
  std::same_as<T, Byte> || std::same_as<T, Word>;

namespace Isa 
{

constexpr std::size_t kNumRegs = 32;
constexpr std::size_t kRamSize = 1024;

// Opcode in [5:0]
constexpr std::uint8_t kOpcodeBdep    = 0b011110;
constexpr std::uint8_t kOpcodeNor     = 0b001010;
constexpr std::uint8_t kOpcodeCls     = 0b110111;
constexpr std::uint8_t kOpcodeSyscall = 0b100011;
constexpr std::uint8_t kOpcodeAdd     = 0b011011;

// Opcode in [31:26]
constexpr std::uint8_t kOpcodeSsat = 0b100000;
constexpr std::uint8_t kOpcodeBeq  = 0b010010;
constexpr std::uint8_t kOpcodeLd   = 0b000010;
constexpr std::uint8_t kOpcodeCbit = 0b011111;
constexpr std::uint8_t kOpcodeJ    = 0b110010;
constexpr std::uint8_t kOpcodeAddi = 0b111011;
constexpr std::uint8_t kOpcodeJalr = 0b101001;
constexpr std::uint8_t kOpcodeSt   = 0b101100;
constexpr std::uint8_t kOpcodeStp  = 0b111000;
constexpr std::uint8_t kOpcodeLi   = 0b011001;


enum Registers : Byte 
{
    x0  = 0,  x1  = 1,  x2  = 2,  x3  = 3,
    x4  = 4,  x5  = 5,  x6  = 6,  x7  = 7,
    x8  = 8,  x9  = 9,  x10 = 10, x11 = 11,
    x12 = 12, x13 = 13, x14 = 14, x15 = 15,
    x16 = 16, x17 = 17, x18 = 18, x19 = 19,
    x20 = 20, x21 = 21, x22 = 22, x23 = 23,
    x24 = 24, x25 = 25, x26 = 26, x27 = 27,
    x28 = 28, x29 = 29, x30 = 30, x31 = 31
};

enum SyscallCodes : Reg 
{
  kSyscallExit = 0x5d
};

} // namespace Isa
