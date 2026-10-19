## Benchmarks
Program built with release config.
Measured with [hyperfine](https://github.com/sharkdp/hyperfine) (1 warmup run, 5 measured runs).

Device: Lenovo X270, Intel i5-6300U

### Branch-heavy

`branches.S`

| Mode | Time, s |
| ---- | ---- |
| Interpreter  (`--mode=int`)| 1.30 ±  0.02  |
| Interpreter-cached  (`--mode=int-cached`)| 2.35 ± 0.03 |
| Threaded (`--mode=thread`) | 1.43 ±  0.01 |

`branches-long.S`

| Mode | Time, s |
| ---- | ---- |
| Interpreter  (`--mode=int`)| 4.32 ±  0.01 |
| Interpreter-cached  (`--mode=int-cached`)| 3.77 ±  0.06  |
| Threaded (`--mode=thread`) | 4.24 ±  0.01 |

### Compute-bound

`crc32.S`

| Mode | Time, ms |
| ---- | ---- |
| Interpreter  (`--mode=int`)| 324 ± 4 |
| Interpreter-cached  (`--mode=int-cached`)| 383 ± 4 |
| Threaded (`--mode=thread`) | 390 ± 9 |

