# Interpreter

## Build & Run

### Prerequisites

| Packet | Version |
| ------ | ------- |
| make   | >= 4    |
| g++    | >= 12   |
| gtest  |         |
| ruby   | >= 3.4  |
| [cxxopts](https://github.com/jarro2783/cxxopts) | >= 3.3.1 |

### Build interpreter

```shell
make all
```

### Run interpreter
```shell
./<BUILD_DIR>/interpreter.o [OPTION] <binary input>
```
For option list, see `help`.

### Run assembler
```shell
ruby as.rb <listing> <binary_output>
```

## Tests

```shell
make test
```
