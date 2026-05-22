# GraphML export

**CircuitGenGraph** supports three GraphML variants for interchange with external tools and the **Generator** repository.

## Formats

| Format | Methods | Purpose |
|--------|---------|---------|
| **Classic** | `toGraphMLClassic()` | Human-readable node types (`input`, `and`, `sequential/ff`, …) |
| **PseudoABCD** | `toGraphMLPseudoABCD()` | Compact numeric `node_type` (ML pipelines) |
| **OpenABCD** | `toGraphMLOpenABCD()` | Extended ABCD: `node_id`, `node_type`, `num_inverted_predecessors`, `edge_type`; graph is **unrolled** (`unrollGraph`) when it contains subgraphs |

XML templates live in `src/GraphMLTemplates.hpp`. File output uses `std::ofstream` overloads (generation timestamp comment is prepended).

## Classic GraphML

Each node has attribute `type` (key `t`):

| Vertex | `type` value |
|--------|----------------|
| Input / output | `input`, `output` |
| Constant | value character (`0`, `1`, …) |
| Gate | `GraphUtils::parseGateToString` name (`and`, `nand`, …) |
| Sequential | `sequential/ff` or `sequential/latch` |
| Subgraph | `subGraph` vertices are skipped; use ABCD formats when flattened subgraph connectivity is required |

Edges are directed `<edge source="…" target="…"/>`. Sequential details: [Sequential.md](Sequential.md).

## PseudoABCD and OpenABCD

Gate codes (`gateToABCDType` in `GraphMLTemplates.hpp`):

| Gate | `node_type` |
|------|-------------|
| AND | 2 |
| NAND | 10 |
| OR | 13 |
| NOR | 14 |
| NOT | 12 |
| BUF | 11 |
| XOR | 15 |
| XNOR | 16 |

Additional codes:

| Vertex | `node_type` |
|--------|-------------|
| Input (OpenABCD only) | 0 |
| Output (OpenABCD only) | 1 |
| Constant | `100` + digit (`1000`, `1001`, …) |
| Flip-flop | 17 |
| Latch | 18 |

When subgraphs exist, **PseudoABCD** and **OpenABCD** first call `unrollGraph()`. **OpenABCD** also skips BUF/NOT nodes (inversion on edges via `num_inverted_predecessors`). **PseudoABCD** uses simplified node/edge templates without `edge_type`.

## Generator integration

When saving a design, Generator may request one or more formats (`getMakeGraphMLClassic()`, `getMakeGraphMLPseudoABCD()`, `getMakeGraphMLOpenABCD()`). All are produced from the same `OrientedGraph`.

## See also

- [FUNCTIONALITY.md](FUNCTIONALITY.md) — overview  
- [Subgraphs.md](Subgraphs.md) — hierarchy and unrolling  
- [BUILDING.md](BUILDING.md) — build and `examples/` (`example_export_graphml_*`)

**Русский:** [GraphML.md](../ru/GraphML.md)
