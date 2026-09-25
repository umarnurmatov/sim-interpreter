#include "gtest/gtest.h"

#include <fstream>

#include "cpu.hpp"

class CpuTestFixture : public testing::Test
{
private:
  Cpu m_cpu;
};

struct InstrTestValue {
  std::vector<Instr> cmds;
};

class InstrTestParametrizedFixture : public CpuTestFixture,
                                     public testing::WithParamInterface<InstrTestValue>

{
};

TEST_P(InstrTestParametrizedFixture, fixme)
{
}

std::vector<InstrTestValue> read_test_value(std::string bin_fname, std::string data_fname)
{
  std::ifstream input_bin(bin_fname);
  std::ifstream input_data(data_fname);


  return {};
}

INSTANTIATE_TEST_SUITE_P(BdepTest, InstrTestParametrizedFixture, testing::ValuesIn(read_test_value("test_bdep.bin", "test_bdep.dat")));
