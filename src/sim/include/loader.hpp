#pragma once

#include <string>

#include <cstdint>
#include <elfio/elfio.hpp>

#include "base_executor.hpp"

class Loader {
public:
  static int load_elf(BaseExecutor* exec, std::string filename);
};
