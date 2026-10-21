add_library(sim_flags INTERFACE)

target_compile_features(sim_flags
  INTERFACE
    cxx_std_20
)

target_compile_options(sim_flags
  INTERFACE
    -fdiagnostics-color=always

    -Wno-interference-size

    -Wall
    -Wextra
    -Wpedantic
    # -Wshadow
    # -Wconversion
    # -Wsign-conversion
    -Wunused
    -Wformat=2
    -Wnon-virtual-dtor
    -march=native

    -fstack-protector-strong # inserts checks for buffer overflow
    # -fPIE -pie
    
    $<$<CONFIG:Debug>:
      -Og
      -g3
      -ggdb
      -fsanitize=address,leak,undefined
      -fno-omit-frame-pointer
    >

    $<$<CONFIG:Release>:
      -g
      -O2
      # -flto
      -DNDEBUG
    >
)

target_link_options(sim_flags
  INTERFACE
    -fdiagnostics-color=always

    -Wall
    -Wextra
    # -fPIE -pie
    -rdynamic
    -march=native

    $<$<CONFIG:Debug>:
      -Og
      -g3
      -ggdb
      -fsanitize=address,leak,undefined
      -fno-omit-frame-pointer
    >

    $<$<CONFIG:Release>:
      -g
      -O2
      # -flto
    >
)
