#pragma once
#include "isa.hpp"

std::size_t cnt_lead_ones(Word rg);
Word sgn_satr(Word rg, Word n);
Word sgn_extend(Word wd, Word bitw);
void clr_bit(Word &wd, std::uint8_t pos);
Word bit_deposit(Word rg, Word mask);
