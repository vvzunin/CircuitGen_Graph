# Sequential logic

Sequential behavior is modeled with **`GraphVertexSequential`** vertices. The sequential kind is encoded with flag bits:

* EN = 1
* SET = 2
* CLR = 4
* RST = 8
* ASYNC = 32
* NEGEDGE = 64
* ff = 16
* latch = 1

Note: the `latch` flag is effectively an alias for `EN` — a latch must have an enable. It cannot use a dedicated clock (level-sensitive to all inputs). **`RST` and `CLR` must not be used together.**

Other `SequentialTypes` enum values combine these flags (each flag is a power of two). A latch cannot combine `ASYNC` or `NEGEDGE`.

## Naming

Enum names encode active flags (first letter per flag). With `ASYNC` and `NEGEDGE`, `n` (negedge clock) and `a` (async reset) appear before the base name (`ff` → `nff`, `aff`, …). Async reset applies only with `RST`.

**Port order matters.** For `nffre` (negedge clock, reset, enable), after clock and data you must pass `EN` then `RST`. This matches the `addSequential` overloads and the internal input order (`data`, `clk`, `en`, `rst/clr`, `set`). If the type expects more or fewer ports than provided, a warning is logged (or printed) naming the missing flag.

The full enum is in `include/CircuitGenGraph/GraphUtils.hpp`. The overload accepting up to **five** signals (clock, data, reset, set, enable) creates **flip-flops only**; simpler overloads accept down to two signals (clock/enable for latches plus data).

## Example (async reset + enable)

```cpp
GraphPtr graph = std::make_shared<OrientedGraph>();

auto *clk = graph->addInput("clk");
auto *data = graph->addInput("data");
auto *rst_n = graph->addInput("rst_n");
auto *en = graph->addInput("en");
auto *seq = graph->addSequential(affre, clk, data, en, rst_n, "q");

auto *out = graph->addOutput("res");
graph->addEdge(seq, out);

graph->toVerilog("./", "example.v");
```

Generated Verilog (abbreviated):

```verilog
module graph_0(
    clk, data, rst_n, en,
    res
);
    input clk, data, rst_n, en;
    output res;
    reg q;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) q <= 1'b0;
        else if (en) q <= data;
    end

    assign res = q;
endmodule
```

### GraphML and DOT export

Classic GraphML uses `sequential/ff` or `sequential/latch` as the node `type`. PseudoABCD and OpenABCD use `node_type` **17** (flip-flop) and **18** (latch). DOT export appends `(ff)` or `(latch)` after the instance name.

See [GraphML.md](GraphML.md) for numeric codes.

**Русский:** [Sequential.md](../ru/Sequential.md)
