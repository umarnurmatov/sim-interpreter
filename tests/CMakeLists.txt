find_package(GTest REQUIRED)
include(GoogleTest)

set(TEST_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/test-data")
add_custom_target(test_binaries
  COMMAND bash "${CMAKE_CURRENT_SOURCE_DIR}/prepare.sh"
  "${PROJECT_SOURCE_DIR}" "${TEST_BINARY_DIR}"
)

add_executable(sim_test 
  interpreter_test.cpp
)

add_dependencies(sim_test 
    test_binaries
)

target_link_libraries(sim_test 
  PRIVATE
    sim_flags
    sim_static 
    GTest::gtest_main
)

target_compile_definitions(sim_test 
  PRIVATE 
    TEST_BUILD_DIR="${TEST_BINARY_DIR}"
)

gtest_discover_tests(sim_test)
