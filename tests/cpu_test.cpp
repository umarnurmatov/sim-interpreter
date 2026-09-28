#include "gtest/gtest.h"

#include <fstream>

#include "cpu.hpp"

struct InstrTestVal {
  std::vector<std::byte> program;
  std::size_t instr_cnt;

  std::size_t rg; 
  Reg expected;    // expected value at Register[rg]
  Reg expected_pc; 
};

class CpuTestFixture : public testing::Test
{
protected:
  Cpu m_cpu;
};

class InstrTestParametrizedFixture : public CpuTestFixture,
                                     public testing::WithParamInterface<InstrTestVal>

{
};

static std::vector<InstrTestVal> read_test_val(std::string bin_fname, std::string data_fname);

TEST_P(InstrTestParametrizedFixture, RegisterResult)
{
  const auto &value = GetParam();
  m_cpu.load_binary(value.program);
  Cpu::BasicBlk blk {};
  m_cpu.prefetch_basic_block(blk, value.instr_cnt);
  m_cpu.exec_block(blk);

  EXPECT_EQ(m_cpu.get_reg(value.rg), value.expected);
  EXPECT_EQ(m_cpu.pc(), value.expected_pc);
}

TEST_F(CpuTestFixture, SyscallTrap)
{
  auto value = read_test_val(
    TEST_BUILD_DIR "/test_syscall.bin", "tests/data/test_syscall.dat").front();
  m_cpu.load_binary(value.program);
  Cpu::BasicBlk blk {};
  m_cpu.prefetch_basic_block(blk);

  try {
    m_cpu.exec_block(blk);
    FAIL() << "Expected SyscallTrap";
  }

  catch (const SyscallTrap &trap) { }

  EXPECT_EQ(m_cpu.get_reg(value.rg), value.expected);
  EXPECT_EQ(m_cpu.pc(), value.expected_pc);
}


// Each .dat row: instruction count, register index, expected unsigned value, pc.
static std::vector<InstrTestVal> read_test_val(std::string bin_fname, std::string data_fname)
{
  std::ifstream input_bin(bin_fname, std::ios::binary);
  std::ifstream input_data(data_fname);
  if (!input_bin || !input_data) {
    throw std::runtime_error("Could not open " + bin_fname + " or " + data_fname);
  }
  input_bin.exceptions(std::ios::failbit | std::ios::badbit);

  std::vector<InstrTestVal> vals;
  InstrTestVal val;
  while (input_data >> val.instr_cnt >> val.rg >> val.expected >> val.expected_pc) {
    val.program.resize(val.instr_cnt * sizeof(Word));

    input_bin.read(
      reinterpret_cast<char*>(val.program.data()), 
      sizeof(Word)*val.instr_cnt);

    vals.push_back(val);
  }

  return vals;
}

INSTANTIATE_TEST_SUITE_P(BdepTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_bdep.bin", "tests/data/test_bdep.dat")));

INSTANTIATE_TEST_SUITE_P(AddTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_add.bin", "tests/data/test_add.dat")));

INSTANTIATE_TEST_SUITE_P(NorTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_nor.bin", "tests/data/test_nor.dat")));

INSTANTIATE_TEST_SUITE_P(ClsTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_cls.bin", "tests/data/test_cls.dat")));

INSTANTIATE_TEST_SUITE_P(SsatTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_ssat.bin", "tests/data/test_ssat.dat")));

INSTANTIATE_TEST_SUITE_P(CbitTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_cbit.bin", "tests/data/test_cbit.dat")));

INSTANTIATE_TEST_SUITE_P(AddiTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_addi.bin", "tests/data/test_addi.dat")));

INSTANTIATE_TEST_SUITE_P(LiTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_li.bin", "tests/data/test_li.dat")));

INSTANTIATE_TEST_SUITE_P(BeqTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_beq.bin", "tests/data/test_beq.dat")));

INSTANTIATE_TEST_SUITE_P(JTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_j.bin", "tests/data/test_j.dat")));

INSTANTIATE_TEST_SUITE_P(JalrTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_jalr.bin", "tests/data/test_jalr.dat")));

INSTANTIATE_TEST_SUITE_P(LdTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_ld.bin", "tests/data/test_ld.dat")));

INSTANTIATE_TEST_SUITE_P(StTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_st.bin", "tests/data/test_st.dat")));

INSTANTIATE_TEST_SUITE_P(StpTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_val(TEST_BUILD_DIR "/test_stp.bin", "tests/data/test_stp.dat")));
