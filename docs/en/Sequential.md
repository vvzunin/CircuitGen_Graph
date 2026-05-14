# Sequential logic

`GraphVertexSequential` vertices support sequential behavior. Flags (bit semantics) include `EN`, `SET`, `CLR`, `RST`, `ASYNC`, `NEGEDGE`, `ff`, `latch`, and combinations formed by bitwise OR. Type names encode which flags apply; see `GraphUtils.hpp` under `include/CircuitGenGraph` for the full enum.

Example (async reset + enable) and generated Verilog are documented in detail in the Russian version.

### Export (GraphML and DOT)

Classic GraphML stores the sequential vertex `type` as `sequential/ff` or `sequential/latch`. PseudoABCD and OpenABCD GraphML use node_type codes **17** (flip-flop) and **18** (latch). DOT export appends `(ff)` or `(latch)` after the instance name.

**Русский:** [Sequential.md](../ru/Sequential.md)
