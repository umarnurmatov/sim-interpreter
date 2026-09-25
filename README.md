# Interpreter

## Build & Run

### Prerequisites

| Packet | Version |
| ------ | ------- |
| make   | >= 4    |
| g++    | >= 12   |
| gtest  |         |
| ruby   | >= 3.4  |

### Build interpreter

```shell
make all
```

### Run interpreter
```shell
./<BUILD_DIR>/interpreter.o <binary_input>
```

### Run assembler
```shell
ruby as.rb <listing> <binary_output>
```

## Tests

```shell
make test
```
