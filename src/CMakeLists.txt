set(SRCS
  base_executor.cpp
  bitops.cpp
  instr.cpp
  interp_executor.cpp
  thread_executor.cpp)

add_library(sim_static 
  STATIC ${SRCS}
)

target_include_directories(sim_static 
  PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(sim_static 
  PUBLIC 
    sim_flags
    cxxopts 
    elfio
)

