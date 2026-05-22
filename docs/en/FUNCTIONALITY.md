# CircuitGen Graph — functionality overview

**Repository:** [CircuitGen_Graph](https://hub.mos.ru/circuitgen/CircuitGen_Graph)  
**CMake project:** `CircuitGenGraph`  
**Role in CircuitGen:** core C++ library for representing digital circuits as directed graphs and importing/exporting exchange formats.

## Purpose

Graph is the data-model core for circuit generation and analysis. A design is an **oriented graph** (`OrientedGraph`): vertices are circuit elements (inputs, outputs, constants, logic gates, sequential cells, subgraphs); edges are connections. **Generator** builds on this model; **Parameters** consumes generated RTL for optimization and physical synthesis.

## Main features

### Graph model

- Create and modify graphs: vertices, edges, hierarchical **subgraphs** (`subGraph`).
- Vertex types (`VertexTypes`): input/output, constant, gate (`Gates`), sequential logic, bus, subgraph.
- **Combinational** and **sequential** logic (flip-flops, latches; see [Sequential.md](Sequential.md)).
- Level updates (topological depth), gate/edge counts, structural hashing.
- Memory and context via `GraphMemory` and `GraphUtils`.

### Export and visualization

- **Verilog** — RTL and hierarchical modules (`toVerilog`).
- **DOT** — Graphviz output (`toDOT`).
- **GraphML** — interchange with external graph tools (used with Generator).
- Verilog module parameters for subgraphs: parse/store `parameter` / `localparam` when binding external Verilog to a subgraph vertex.

### Import

- **Verilog** import via **lorina** (`GraphReader`, submodule `lib/lorina`).
- Multi-module parsing context (`Context`) for hierarchical designs.
- Details: [Lorina](https://hub.mos.ru/circuitgen/lorina) repository, `docs/en/FUNCTIONALITY.md`.

### Graph-level simulation

- Truth tables for logic operations (`enums.hpp`, `Gates`).
- Value propagation on vertices for combinational checks.

### Testbench generation

- `TestbenchGenerator` API — automatic Verilog testbenches (combinational and partial sequential flows).
- Examples and tests with **Icarus Verilog** (`iverilog` / `vvp`).

### Logging

- `CG_LOG_*` macros over **easylogging++** (or stubs when logging is disabled).
- Initialization via `OrientedGraph::initLogging`.

## Public API (key headers)

| Header | Purpose |
|--------|---------|
| `OrientedGraph.hpp` | Main graph class, export, hierarchy |
| `GraphVertex*.hpp` | Vertex types (input, output, gate, sequential, subgraph, …) |
| `GraphReader.hpp` | Verilog import |
| `TestbenchGenerator.hpp` | Testbench generation |
| `GraphUtils.hpp`, `enums.hpp` | Utilities, enums, truth tables |
| `Logging.hpp` | Logging |

## Build and artifacts

- CMake 3.26+, C++17, presets `dev`, `release-ci`, etc.
- Static/shared library `CircuitGenGraph`; optional `examples/` and `test/`.
- Docs: Doxygen (HTML/PDF, ru/en), Synology NAS deploy from CI (`scripts/docs/deploy-synology.sh`).

## Dependencies

- **lorina** ([circuitgen/lorina](https://hub.mos.ru/circuitgen/lorina), submodule `lib/lorina`) — Verilog parsing (Verilog reader only in Graph).
- **easylogging++** — logging.
- Docs/CI: Doxygen, LaTeX, Ninja, clang-format, lcov, etc. (`scripts/setup/install-deps-*.sh`).

## Related repositories

| Repository | Relationship |
|------------|--------------|
| **Generator** | Produces `std::shared_ptr<OrientedGraph>`; writes Verilog/GraphML |
| **Parameters** | Consumes Verilog for Yosys/ABC/OpenLane |
| **CircuitGen** (main) | End-to-end project workflow |

## Who should use this repo

- Generator authors needing a shared circuit representation and Verilog export.
- Researchers working with hierarchical RTL, module import, and automated verification (testbenches).
- EDA integrators connecting CircuitGen to simulators and synthesizers.

## See also

- [README.en.md](../../README.en.md) — quick start  
- [BUILDING.md](BUILDING.md) — presets and CI parity  
- [HACKING.md](HACKING.md) — developer workflow  
- [Sequential.md](Sequential.md) · [GraphML.md](GraphML.md) · [VerilogImport.md](VerilogImport.md) · [Subgraphs.md](Subgraphs.md) · [TestbenchGenerator.md](TestbenchGenerator.md) · [MEMORY_MANAGEMENT.md](MEMORY_MANAGEMENT.md)

**Русский:** [FUNCTIONALITY.md](../ru/FUNCTIONALITY.md)
