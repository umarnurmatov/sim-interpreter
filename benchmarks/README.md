### Branch-heavy
`branches.S`; program built with release config.
Measured with [`hyperfine`](ttps://github.com/sharkdp/hyperfine) (1 warmup run, 5 measured runs).

| Mode | Time, ms |
| ---- | ---- |
| Interpreter  (`--mode=int`)| 1292 ± 12.0 |
| Threaded (`--mode=thread`) | 723.2 ± 31.8 |

