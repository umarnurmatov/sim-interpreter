#include "bitops.hpp"

#include <algorithm>
#include <cassert>

#include <immintrin.h>

std::size_t cnt_lead_ones(Word rg)
{
  #if defined(__GNUC__)
    return rg == UINT32_MAX 
                 ? Isa::kRegWidth 
                 : static_cast<std::size_t>(__builtin_clz(~rg));
  #else
    std::size_t cnt = 0;
    while (rg & (1u << (Isa::kRegWidth - 1))) {
        ++n;
        rg <<= 1;
    }
    return cnt;
  #endif
}

// assuming n <= 31
Word sgn_satr(Word rg, Word n)
{
  assert(n < Isa::kWordWidth);

  int32_t lower_b = -(1 << (n-1));
  int32_t upper_b = (1 << (n-1)) - 1;
  int32_t res = std::clamp<int32_t>(
    std::bit_cast<int32_t>(rg), 
    lower_b, 
    upper_b);

  return std::bit_cast<Word>(res);
}

// assuming bitw <= 31
Word sgn_extend(Word wd, Word bitw)
{
  assert(bitw < Isa::kWordWidth);

  const Word m = 1u << (bitw - 1);   
  return (wd ^ m) - m;
}

void clr_bit(Word &wd, std::uint8_t pos)
{
  wd &= ~(1u << pos);
}

Word bit_deposit(Word rg, Word mask)
{
  if (__builtin_cpu_supports("bmi2")) {
    return _pdep_u32(rg, mask);
  }

  Word res {};
  for (std::uint32_t bit = 1; mask; bit <<= 1) {
      std::uint32_t mask_lsb = mask & -mask;
      if (rg & bit)
          res |= mask_lsb;
      mask &= mask - 1;
  }
  return res; 
}
