#include <cstdlib>
#include <iostream>
#include <fstream>

#include "interpreter.hpp"

int main(int argc, char* argv[])
{
  Interpreter intrp {};

  if (argc < 2) {
    std::cerr << "usage: intrp <input filename>" << std::endl;
    return EXIT_FAILURE;
  }
  
  std::ifstream file(argv[1], std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "could not open " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }

  while (!file.eof()) {
    Word wd {};
    file.read(reinterpret_cast<char*>(&wd), sizeof wd);
    intrp.tick(wd);
  }

  return EXIT_SUCCESS;
}
