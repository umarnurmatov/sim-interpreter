include(FetchContent)

FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG        v3.3.1
)

FetchContent_Declare(
  elfio
  GIT_REPOSITORY https://github.com/serge1/ELFIO.git
  GIT_TAG        Release_3.12
)

FetchContent_Declare(
  rice
  GIT_REPOSITORY https://github.com/ruby-rice/rice.git
  GIT_TAG        4.12.0
)

FetchContent_Declare(
  asmjit
  GIT_REPOSITORY https://github.com/asmjit/asmjit.git
)

set(ASMJIT_NO_FOREIGN TRUE)
set(ASMJIT_EMBED TRUE)
set(ASMJIT_BUILD_RELEASE TRUE)

FetchContent_MakeAvailable(
  cxxopts 
  elfio
  rice
  asmjit
)
