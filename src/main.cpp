#include <cstdlib>
#include <iostream>

#include "interpreter.hpp"

int main(int argc, char* argv[])
{
  Interpreter intrp {};

  if (argc < 2) {
    std::cerr << "usage: intrp <input filename>" << std::endl;
    return EXIT_FAILURE;
  }
  
  if (int res = intrp.load_binary_file(argv[1]); res != 0)
    return res;

  while(!intrp.halted()) {
    intrp.tick();
  }

  return EXIT_SUCCESS;
}
