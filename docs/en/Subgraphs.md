# Subgraphs and hierarchy

Hierarchical designs use nested **`OrientedGraph`** objects and **`GraphVertexSubGraph`** vertices.

## Adding a subgraph

```cpp
auto parent = std::make_shared<OrientedGraph>("top");
auto child  = std::make_shared<OrientedGraph>("child");

auto inA = child->addInput("a");
auto outY = child->addOutput("y");
auto *buf = child->addGate(GateBuf, "buf");
child->addEdge(inA, buf);
child->addEdge(buf, outY);

std::vector<VertexPtr> bindings = { parent->addInput("x") };
std::vector<VertexPtr> outputs = parent->addSubGraph(child, bindings);
```

- `addSubGraph` registers `child` in the parent’s `d_subGraphs` and creates a subgraph instance vertex.
- `getSubGraphs()` lists nested graphs.

## External Verilog file

Bind a standalone RTL file to a subgraph vertex:

```cpp
sgVertex->setVerilogPath("/path/to/child_module.v");
```

Export can pull the module from disk. Parameters are stored on the graph:

- `setVerilogParameter` / `getVerilogParameters` / `clearVerilogParameters`

Supports `parameter` and `localparam` parsing (`GraphVertexSubGraph`).

## Unrolling

`toVerilog`, `toDOT`, and **OpenABCD GraphML** may need a flat netlist: **`unrollGraph()`** inlines subgraph hierarchy into one graph.

## Export and GraphML

- **Classic** skips `subGraph` vertices as standalone nodes; use it for flat graphs or top-level views that do not need flattened hierarchy.
- **PseudoABCD** and **OpenABCD** call `unrollGraph()` before serialization when the graph contains subgraphs.

See [GraphML.md](GraphML.md).

## Vertex lookup

`getVerticesByName(..., i_addSubGraphs)` searches inside subgraphs when the flag is true.

## See also

- [VerilogImport.md](VerilogImport.md)  
- [FUNCTIONALITY.md](FUNCTIONALITY.md)  
- Tests under `test/` (`SubGraph`, `toVerilog`, `toDOT`)

**Русский:** [Subgraphs.md](../ru/Subgraphs.md)
