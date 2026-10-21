include(FetchContent)

FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG        44380e5a44706ab7347f400698c703eb2a196202 # v3.3.1
)

FetchContent_Declare(
  elfio
  GIT_REPOSITORY https://github.com/serge1/ELFIO.git
  GIT_TAG        8ae6cec5d60495822ecd57d736f66149da9b1830 # v3.12
)

FetchContent_MakeAvailable(cxxopts elfio)
