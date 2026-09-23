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
