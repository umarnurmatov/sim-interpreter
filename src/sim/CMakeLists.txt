set(SRCS
  src/base_executor.cpp
  src/bitops.cpp
  src/instr.cpp
  src/interp_executor.cpp
  src/thread_executor.cpp)

add_library(sim_static 
  STATIC ${SRCS}
)

target_include_directories(sim_static 
  PUBLIC include
)

target_link_libraries(sim_static 
  PUBLIC 
    sim_flags
    cxxopts 
    elfio
)

