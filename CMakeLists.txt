cmake_minimum_required(VERSION 3.11)
project("simulator")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()
  
add_subdirectory(flags)
add_subdirectory(deps)
add_subdirectory(src)
add_subdirectory(tests)

add_executable(sim.x 
  src/main.cpp
)

target_link_libraries(sim.x
  PRIVATE 
    sim_flags
    sim_static
)

