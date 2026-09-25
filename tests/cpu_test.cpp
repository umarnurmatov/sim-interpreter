#include "gtest/gtest.h"

#include <fstream>
#include <stdexcept>

#include "cpu.hpp"

#ifndef TEST_BUILD_DIR
#define TEST_BUILD_DIR "build"
#endif

class CpuTestFixture : public testing::Test
{
protected:
  Cpu m_cpu;
};

struct InstrTestValue {
  std::vector<Instr> cmds;
  std::size_t reg;
  Reg expected;
  Reg expected_pc;
};

class InstrTestParametrizedFixture : public CpuTestFixture,
                                     public testing::WithParamInterface<InstrTestValue>

{
};

TEST_P(InstrTestParametrizedFixture, RegisterResult)
{
  const auto &value = GetParam();
  for (Instr cmd : value.cmds) {
    m_cpu.exec(cmd);
  }
  EXPECT_EQ(m_cpu.get_reg(value.reg), value.expected);
  EXPECT_EQ(m_cpu.pc(), value.expected_pc);
}

// Each .dat row: instruction count, register index, expected unsigned value, PC.
std::vector<InstrTestValue> read_test_value(std::string bin_fname, std::string data_fname)
{
  std::ifstream input_bin(bin_fname, std::ios::binary);
  std::ifstream input_data(data_fname);
  if (!input_bin || !input_data) {
    throw std::runtime_error("Cannot open " + bin_fname + " or " + data_fname);
  }
  input_bin.exceptions(std::ios::failbit | std::ios::badbit);

  Cpu decoder;
  std::vector<InstrTestValue> values;
  std::size_t count;
  InstrTestValue value;
  while (input_data >> count >> value.reg >> value.expected >> value.expected_pc) {
    value.cmds.clear();
    for (std::size_t i = 0; i < count; ++i) {
      Word enc;
      input_bin.read(reinterpret_cast<char*>(&enc), sizeof(enc));
      value.cmds.push_back(decoder.decode(enc));
    }
    values.push_back(value);
  }

  return values;
}

TEST_F(CpuTestFixture, Fibonacci)
{
  std::ifstream data("tests/data/test_fibonacci.dat");
  std::size_t count;
  InstrTestValue value;
  ASSERT_TRUE(data >> count >> value.reg >> value.expected >> value.expected_pc);

  std::ifstream binary(TEST_BUILD_DIR "/test_fibonacci.bin", std::ios::binary);
  std::vector<std::byte> program(count * sizeof(Word));
  ASSERT_TRUE(binary.read(reinterpret_cast<char*>(program.data()), program.size()));
  m_cpu.load_binary(program);

  for (std::size_t steps = 0; steps < 1000 && m_cpu.pc() != value.expected_pc; ++steps) {
    m_cpu.exec(m_cpu.decode(m_cpu.fetch()));
  }
  EXPECT_EQ(m_cpu.pc(), value.expected_pc);
  EXPECT_EQ(m_cpu.get_reg(value.reg), value.expected);
}

TEST_F(CpuTestFixture, SyscallTrap)
{
  const auto value = read_test_value(
    TEST_BUILD_DIR "/test_syscall.bin", "tests/data/test_syscall.dat").front();
  for (std::size_t i = 0; i + 1 < value.cmds.size(); ++i)
    m_cpu.exec(value.cmds[i]);

  try {
    m_cpu.exec(value.cmds.back());
    FAIL() << "Expected SyscallTrap";
  }
  catch (const SyscallTrap &trap) { }

  EXPECT_EQ(m_cpu.get_reg(value.reg), value.expected);
  EXPECT_EQ(m_cpu.pc(), value.expected_pc);
}

INSTANTIATE_TEST_SUITE_P(BdepTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_bdep.bin", "tests/data/test_bdep.dat")));

INSTANTIATE_TEST_SUITE_P(AddTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_add.bin", "tests/data/test_add.dat")));

INSTANTIATE_TEST_SUITE_P(NorTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_nor.bin", "tests/data/test_nor.dat")));

INSTANTIATE_TEST_SUITE_P(ClsTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_cls.bin", "tests/data/test_cls.dat")));

INSTANTIATE_TEST_SUITE_P(SsatTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_ssat.bin", "tests/data/test_ssat.dat")));

INSTANTIATE_TEST_SUITE_P(CbitTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_cbit.bin", "tests/data/test_cbit.dat")));

INSTANTIATE_TEST_SUITE_P(AddiTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_addi.bin", "tests/data/test_addi.dat")));

INSTANTIATE_TEST_SUITE_P(LiTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_li.bin", "tests/data/test_li.dat")));

INSTANTIATE_TEST_SUITE_P(BeqTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_beq.bin", "tests/data/test_beq.dat")));

INSTANTIATE_TEST_SUITE_P(JTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_j.bin", "tests/data/test_j.dat")));

INSTANTIATE_TEST_SUITE_P(JalrTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_jalr.bin", "tests/data/test_jalr.dat")));

INSTANTIATE_TEST_SUITE_P(LdTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_ld.bin", "tests/data/test_ld.dat")));

INSTANTIATE_TEST_SUITE_P(StTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_st.bin", "tests/data/test_st.dat")));

INSTANTIATE_TEST_SUITE_P(StpTest, InstrTestParametrizedFixture,
  testing::ValuesIn(read_test_value(TEST_BUILD_DIR "/test_stp.bin", "tests/data/test_stp.dat")));
