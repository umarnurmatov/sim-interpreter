# TOY-ISA simulator

ISA description is available in `docs/isa.pdf`. This project includes assembler and simulator.

## Prerequisites

| Packet                                          | Version |
| ----------------------------------------------- | ------- |
| cmake                                           | >= 3.11 |
| g++                                             | >= 12   |
| gtest                                           |         |
| ruby                                            | >= 3.4  |
| [rice](https://github.com/ruby-rice/rice)       | = 4.12.0|
| [cxxopts](https://github.com/jarro2783/cxxopts) | = 3.3.1 |
| [elfio](https://github.com/serge1/ELFIO)        | = 3.12  |

`rice`, `cxxxopts` and `elfio` are managed via `FetchContent`, so there is no need to install them manually.

## Simulator

### Build

```sh
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Run
```sh
./build/sim.x [OPTIONS] <input>
```
For available option list, run with `--help`.

`input` is an ELF object file, see Assembler section.

## Assembler

Assembler produces an ELF object file that's later can be passed to simulator.

### Pre-run

Assembler uses Ruby-to-C++ binding to custom [elfio](https://github.com/serge1/ELFIO) library
and it's neccessary to build it (see Simulator/Build section) before using assembler.

### Run
```sh
ruby -I build/src/elfio_ext as.rb <asm> <output>
```

## Tests
Project includes system tests for every ISA instruction.
To run them, build the project first (or just target `sim_test`) and then
```sh
cd build
ctest
```
