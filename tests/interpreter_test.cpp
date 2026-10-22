#include <gtest/gtest.h> 

#include "interp_executor.hpp"
#include "loader.hpp"

class InterpreterTestFixture : public testing::Test
{
protected:
  InterpExecutor intrp;
};

class ProgramTestParametrizedFixture : public InterpreterTestFixture,
                                       public testing::WithParamInterface<std::string>

{
};

TEST_P(ProgramTestParametrizedFixture, Result)
{
  const auto &bin_fname = GetParam();
  int res = Loader::load_elf(&intrp, bin_fname);
  ASSERT_EQ(res, 0) << "Could not load bin file";

  int exit_code = intrp.run();

  EXPECT_EQ(exit_code, 0) << "Exited with code " << exit_code;
}

INSTANTIATE_TEST_SUITE_P(FibonacciTest, ProgramTestParametrizedFixture, 
  testing::Values(TEST_BUILD_DIR "/test_fibonacci.o"));

INSTANTIATE_TEST_SUITE_P(BdepTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_bdep.o"));

INSTANTIATE_TEST_SUITE_P(AddTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_add.o"));

INSTANTIATE_TEST_SUITE_P(NorTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_nor.o"));

INSTANTIATE_TEST_SUITE_P(ClsTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_cls.o"));

INSTANTIATE_TEST_SUITE_P(SsatTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_ssat.o"));

INSTANTIATE_TEST_SUITE_P(CbitTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_cbit.o"));

INSTANTIATE_TEST_SUITE_P(AddiTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_addi.o"));

INSTANTIATE_TEST_SUITE_P(LiTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_li.o"));

INSTANTIATE_TEST_SUITE_P(BeqTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_beq.o"));

INSTANTIATE_TEST_SUITE_P(JTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_j.o"));

INSTANTIATE_TEST_SUITE_P(JalrTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_jalr.o"));

INSTANTIATE_TEST_SUITE_P(LdTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_ld.o"));

INSTANTIATE_TEST_SUITE_P(StTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_st.o"));

INSTANTIATE_TEST_SUITE_P(StpTest, ProgramTestParametrizedFixture,
  testing::Values(TEST_BUILD_DIR "/test_stp.o"));
