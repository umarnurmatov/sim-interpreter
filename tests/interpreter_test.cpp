#include <gtest/gtest.h> 

#include <fstream>

#include "interpreter.hpp"

struct ProgramTestVal {
  std::string bin_fname;

  std::size_t rg; 
  Reg expected;    // expected value at Register[rg]
};

class InterpreterTestFixture : public testing::Test
{
protected:
  Interpreter intrp;
};

class ProgramTestParametrizedFixture : public InterpreterTestFixture,
                                       public testing::WithParamInterface<ProgramTestVal>

{
};

TEST_P(ProgramTestParametrizedFixture, RegisterResult)
{
  const auto &val = GetParam();
  ASSERT_EQ(intrp.load_binary_file(val.bin_fname), 0) << "Could not load bin file";

  while(!intrp.halted()) {
    intrp.tick();
  }

  EXPECT_EQ(intrp.get_reg(val.rg), val.expected);
}

// Each .dat row: instruction count, register index, expected unsigned value, pc.
static std::vector<ProgramTestVal> read_test_val(std::string bin_fname, std::string data_fname)
{
  std::ifstream input_data(data_fname);
  if (!input_data)
    throw std::runtime_error("Could not open " + data_fname);
  
  std::vector<ProgramTestVal> test_vals;
  ProgramTestVal val { .bin_fname = bin_fname };
  while (input_data >> val.rg >> val.expected)
    test_vals.push_back(val);

  return test_vals;
}

INSTANTIATE_TEST_SUITE_P(FibonacciTest, ProgramTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_fibonacci.bin", "tests/data/test_fibonacci.dat")));
