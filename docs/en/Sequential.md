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

---

## Testbench Generation for Sequential Circuits

The `TestbenchGenerator` class automatically detects sequential elements in the graph and uses specialized testbench generation logic for such circuits.

### Automatic Signal Detection

When `TestbenchGenerator` is initialized for a circuit containing `GraphVertexSequential` elements, the following signals are automatically identified:
- **Clock signals** (`clk`) — from each sequential element's `getClk()` method
- **Reset signals** (`rst`, `rst_n`) — from `getRst()`
- **Data inputs** — all remaining inputs that are not control signals

### API Usage

```cpp
GraphPtr graph = std::make_shared<OrientedGraph>();
auto *clk = graph->addInput("clk");
auto *data = graph->addInput("data");
auto *rst_n = graph->addInput("rst_n");
auto *seq = graph->addSequential(affr, clk, data, rst_n, "q");
auto *out = graph->addOutput("res");
graph->addEdge(seq, out);

// Check circuit type
TestbenchConfig config;
config.clockPeriod = 10;      // Clock period
config.resetDuration = 25;    // Reset phase duration
config.resetActiveValue = 0;  // Active-low reset

TestbenchGenerator gen(graph, config);
assert(gen.isSequentialCircuit() == true);

// Generate random test vectors (10 clock cycles)
gen.generateSequentialTestVectors(10, 42);

// Or manually add a vector
gen.addSequentialTestVector(
    {'1'},          // data = 1
    {'1'},          // expected res = 1
    true            // check output
);

// Write testbench to file
gen.toVerilogTestbench("./output/", "my_ff_tb");
```

### Generated Testbench Structure

For sequential circuits, the generated testbench includes:

1. **Signal declarations** — `reg` for inputs (clk, rst, data), `wire` for outputs
2. **Clock generation block** — `always #(PERIOD/2) clk = ~clk;`
3. **DUT instantiation** — connecting all ports
4. **Stimulus block** — `initial begin ... end` with:
   - Initialization of all signals
   - Reset phase (assert → wait → deassert)
   - Data application on clock edges via `@(posedge clk)`
   - Output checking (if expected values are provided)
   - Summary report and `$finish`

### Verification with Icarus Verilog

```cpp
// Full cycle: Verilog generation + testbench + compilation + simulation
auto result = gen.runIcarusVerification("./work_dir");

if (result.success) {
    std::cout << "All tests passed!" << std::endl;
} else {
    std::cerr << "Error: " << result.errorMessage << std::endl;
}
```

The `runIcarusVerification` method automatically:
1. Generates the Verilog module file
2. Generates the testbench
3. Compiles via `iverilog`
4. Runs simulation via `vvp`
5. Parses output and populates `VerificationResult`

**Русский:** [Sequential.md](../ru/Sequential.md)
