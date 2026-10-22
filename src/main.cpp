#include <cstdlib>
#include <iostream>

#include <cxxopts.hpp>

#include "base_executor.hpp"
#include "thread_executor.hpp"
#include "interp_executor.hpp"
#include "loader.hpp"

int main(int argc, char* argv[])
{
  cxxopts::Options opts("toy-asm interpreter, copyright (c) n1njaeng1neer");

  BaseExecutor* executor;

  opts.add_options()
    ("input",  "input file", cxxopts::value<std::string>())
    ("m,mode", "mode",       cxxopts::value<std::string>())
    ("h,help", "help"                                     );
  opts.parse_positional({ "input" });

  auto opts_parsed = opts.parse(argc, argv);

  if (opts_parsed.contains("help")) {
    std::cout << opts.help();
    return EXIT_SUCCESS;
  }
  
  try {
    auto mode = opts_parsed["mode"].as<std::string>();

    if (mode == "int") 
      executor = new InterpExecutor{InterpExecutor::kLinear};
    else if (mode == "int-cached")
      executor = new InterpExecutor{InterpExecutor::kCached};
    else if (mode == "thread")
      executor = new ThreadExecutor{};
    else {
      std::cerr << "mode = <int, int-cached, thread>" << std::endl;
      return EXIT_FAILURE;
    }
    
    auto input = opts_parsed["input"].as<std::string>();

    if (int res = Loader::load_elf(executor, input); res != 0)
      return EXIT_FAILURE;
  }
  catch (const cxxopts::exceptions::exception& e)
  {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  
  int exit_code = executor->run();
  std::cout << "exited with code " << exit_code << std::endl;

  delete executor;

  return EXIT_SUCCESS;
}

