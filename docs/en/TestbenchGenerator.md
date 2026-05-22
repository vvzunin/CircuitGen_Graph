# Testbench generation

The **`TestbenchGenerator`** API (`include/CircuitGenGraph/TestbenchGenerator.hpp`) emits Verilog testbenches for `OrientedGraph` designs and can run an external simulator.

## Main types

| Type | Role |
|------|------|
| `TestbenchConfig` | `timescale`, clock period, reset, VCD, timeout |
| `TestVectorStrategy` | Stimulus strategy (exhaustive, random, …) |
| `VerificationResult` | Run summary, vectors, simulator output |
| `TestbenchGenerator` | Bench file generation and simulator invocation |

Include after `OrientedGraph.hpp`. Graph logging: `OrientedGraph::initLogging` (root README).

## Scenarios

- **Combinational** designs: truth-table style checks.
- **Sequential** fragments: partial support via clock/reset in `TestbenchConfig`; broader sequential flows are still evolving.

## Simulation (Icarus Verilog)

Tests use **Icarus** (`iverilog`, `vvp`), installed by `scripts/setup/install-deps-*.sh`.

Some GoogleTest cases are prefixed with **`DISABLED_`** so default `ctest` does not require a simulator:

```sh
./build/dev/test/CircuitGenGraph_tests \
  --gtest_filter='TestbenchGeneratorTests.DISABLED_IcarusVerificationAndGate' \
  --gtest_also_run_disabled_tests
```

Full workflow: [BUILDING.md#tests-and-icarus](BUILDING.md#tests-and-icarus).

## Demo program

Source: `examples/example_testbench.cpp` (requires `CircuitGenGraph_BUILD_EXAMPLES=ON`, **dev** preset).

```sh
cmake --preset=dev
cmake --build --preset=dev -j "$(nproc)"
./build/dev/examples/example_testbench
```

## See also

- [README.en.md](../../README.en.md) — overview  
- [Sequential.md](Sequential.md)  
- [BUILDING.md](BUILDING.md)

**Русский:** [TestbenchGenerator.md](../ru/TestbenchGenerator.md)
