cmake_minimum_required(VERSION 3.17)
project("simulator")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()
  
add_subdirectory(flags)
add_subdirectory(deps)
add_subdirectory(src/sim)
add_subdirectory(src/elfio_ext)
add_subdirectory(tests)

add_executable(sim.x 
  src/main.cpp
)

target_link_libraries(sim.x
  PRIVATE 
    sim_flags
    sim_static
)
