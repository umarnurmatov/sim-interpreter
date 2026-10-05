#include <gtest/gtest.h> 

#include <fstream>

#include "interpreter.hpp"

class InterpreterTestFixture : public testing::Test
{
protected:
  Interpreter intrp;
};

class ProgramTestParametrizedFixture : public InterpreterTestFixture,
                                       public testing::WithParamInterface<std::string>

{
};

TEST_P(ProgramTestParametrizedFixture, Result)
{
  const auto &bin_fname = GetParam();
  ASSERT_EQ(intrp.load_binary_file(bin_fname), 0) << "Could not load bin file";

  while(!intrp.halted()) {
    intrp.tick();
  }

  EXPECT_EQ(intrp.exit_code(), 0) << "Exited with code " << intrp.exit_code();
}

INSTANTIATE_TEST_SUITE_P(FibonacciTest, ProgramTestParametrizedFixture, 
  testing::Values(TEST_BUILD_DIR "/test_fibonacci.bin"));

INSTANTIATE_TEST_SUITE_P(BdepTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_bdep.bin"));

INSTANTIATE_TEST_SUITE_P(AddTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_add.bin"));

INSTANTIATE_TEST_SUITE_P(NorTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_nor.bin"));

INSTANTIATE_TEST_SUITE_P(ClsTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_cls.bin"));

INSTANTIATE_TEST_SUITE_P(SsatTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_ssat.bin"));

INSTANTIATE_TEST_SUITE_P(CbitTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_cbit.bin"));

INSTANTIATE_TEST_SUITE_P(AddiTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_addi.bin"));

INSTANTIATE_TEST_SUITE_P(LiTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_li.bin"));

INSTANTIATE_TEST_SUITE_P(BeqTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_beq.bin"));

INSTANTIATE_TEST_SUITE_P(JTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_j.bin"));

INSTANTIATE_TEST_SUITE_P(JalrTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_jalr.bin"));

INSTANTIATE_TEST_SUITE_P(LdTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_ld.bin"));

INSTANTIATE_TEST_SUITE_P(StTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_st.bin"));

INSTANTIATE_TEST_SUITE_P(StpTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_stp.bin"));
