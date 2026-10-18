# TOY-ISA simulator

ISA description is available in `docs/isa.pdf`. This project includes assembler and simulator.

## Simulator

### Prerequisites

| Packet | Version |
| ------ | ------- |
| make   | >= 4    |
| g++    | >= 12   |
| gtest  |         |
| ruby   | >= 3.4  |
| [cxxopts](https://github.com/jarro2783/cxxopts) | >= 3.3.1 |

### Build

Debug build:
```sh
make all
```
Release build:
```sh
make TARGET=Release all
```

### Run
```sh
./<BUILD_DIR>/interpreter.o [OPTIONS] <input>
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
make test
```
