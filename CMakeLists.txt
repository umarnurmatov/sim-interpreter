cmake_minimum_required(VERSION 3.11)
project("simulator")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
  
set(SRCS
  src/base_executor.cpp
  src/bitops.cpp
  src/instr.cpp
  src/interp_executor.cpp
  src/thread_executor.cpp)

add_library(sim_static STATIC ${SRCS})
target_include_directories(sim_static PUBLIC include)
target_compile_options(sim_static PUBLIC -march=native)

add_executable(sim src/main.cpp)
target_link_libraries(sim sim_static)

add_subdirectory(tests)
