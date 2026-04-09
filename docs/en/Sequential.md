# Sequential logic

`GraphVertexSequential` vertices support sequential behavior. Flags (bit semantics) include `EN`, `SET`, `CLR`, `RST`, `ASYNC`, `NEGEDGE`, `ff`, `latch`, and combinations formed by bitwise OR. Type names encode which flags apply; see `GraphUtils.hpp` under `include/CircuitGenGraph` for the full enum.

Example (async reset + enable) and generated Verilog are documented in detail in the Russian version.

**Русский:** [Sequential.md](../ru/Sequential.md)
