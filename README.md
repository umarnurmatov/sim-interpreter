# TOY-ISA simulator

ISA description is available in `docs/isa.pdf`. This project includes assembler and simulator.

## Simulator

### Prerequisites

| Packet                                          | Version |
| ----------------------------------------------- | ------- |
| cmake                                           | >= 3.11 |
| g++                                             | >= 12   |
| gtest                                           |         |
| ruby                                            | >= 3.4  |
| [cxxopts](https://github.com/jarro2783/cxxopts) | = 3.3.1 |
| [elfio](https://github.com/serge1/ELFIO)        | = 3.12  |

`cxxxopts` and `elfio` are managed via `FetchContent`, so there is no need to install them manually.

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

## Assembler 
### Run
```sh
ruby as.rb <asm> <output>
```

## Tests
Project includes system tests for every ISA instruction.
To build & run them, use:
```sh
cd build
ctest
```
