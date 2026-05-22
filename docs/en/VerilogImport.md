# Verilog import

RTL is imported into `OrientedGraph` via **`GraphReader`** ([lorina](https://hub.mos.ru/circuitgen/lorina) callbacks) and **`OrientedGraph::readVerilog`**.

## API

```cpp
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>

CG_Graph::Context context;
CG_Graph::OrientedGraph::readVerilog("/path/to/module.v", context);

for (const auto &[name, graph] : context.d_graphs) { /* … */ }

auto ctx = CG_Graph::OrientedGraph::readVerilog("/path/to/module.v");
```

### `Context`

| Field | Role |
|-------|------|
| `d_currentTopName` | Top module name during hierarchical reads |
| `d_graphs` | Map `module_name` → `GraphPtr` |
| `d_currentGraph` | Graph under construction |
| `d_currentGraphNamesList` | Fast vertex lookup by name |
| `d_numberOfVertices` | Hint for memory reservation |

Reuse the same `Context` across files to wire submodule instances (see `GraphReader.hpp`; multi-file support is evolving).

## Behavior

1. Open the file; scan until the first `module` keyword.
2. Run `lorina::read_verilog` with a `GraphReader` instance.
3. `parse_error` → `std::runtime_error`; missing file → exception with path.

Callbacks cover module headers, ports, wires, parameters, assigns, instances, etc. (`GraphReader.hpp`, `src/GraphReader.cpp`).

## Limitations

- Supported Verilog is the subset covered by lorina and `test/testModulesForReading/`.
- Advanced hierarchy or SystemVerilog may fail — validate on your RTL and extend tests.
- Downstream tools often use `toVerilog` export after editing the graph in memory.

## Tests

`test/src/GraphReaderTests.cpp`, golden `.v` under `test/testModulesForReading/`.

```sh
ctest --preset=dev --output-on-failure
```

## lorina submodule

`lib/lorina` → [circuitgen/lorina](https://hub.mos.ru/circuitgen/lorina). Bump the submodule to pick up parser changes.

## See also

- [FUNCTIONALITY.md](FUNCTIONALITY.md)  
- [Subgraphs.md](Subgraphs.md) — external `.v` on subgraph vertices  
- [Lorina docs](https://hub.mos.ru/circuitgen/lorina/-/blob/main/docs/en/FUNCTIONALITY.md)

**Русский:** [VerilogImport.md](../ru/VerilogImport.md)
