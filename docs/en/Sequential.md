# Sequential logic

`GraphVertexSequential` vertices support sequential behavior. Flags (bit semantics) include `EN`, `SET`, `CLR`, `RST`, `ASYNC`, `NEGEDGE`, `ff`, `latch`, and combinations formed by bitwise OR. Type names encode which flags apply; see `GraphUtils.hpp` under `include/CircuitGenGraph` for the full enum.

Example (async reset + enable) and generated Verilog are documented in detail in the Russian version.

### Export (GraphML and DOT)

Classic GraphML stores the sequential vertex `type` as `sequential/ff` or `sequential/latch`. PseudoABCD and OpenABCD GraphML use node_type codes **17** (flip-flop) and **18** (latch). DOT export appends `(ff)` or `(latch)` after the instance name.

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
