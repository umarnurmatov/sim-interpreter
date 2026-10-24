set(SRCS
  src/bitops.cpp
  src/instr.cpp
  src/base_executor.cpp
  src/interp_executor.cpp
  src/thread_executor.cpp
  src/jit_executor.cpp
  src/loader.cpp)

add_library(sim_static 
  STATIC ${SRCS}
)

target_include_directories(sim_static 
  PUBLIC include
)

target_link_libraries(sim_static 
  PUBLIC 
    sim_flags
    cxxopts::cxxopts
    elfio::elfio
    asmjit::asmjit
)

# cxx_std_20 already propagated through flags
# explicitly added it to force it to compile_commands.json
set_target_properties(sim_static
  PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
