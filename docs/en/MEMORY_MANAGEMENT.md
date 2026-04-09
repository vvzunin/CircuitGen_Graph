# Memory management

This document describes how to pre-allocate memory when working with `OrientedGraph` and vertices.

### Graph initialization

Typically create a graph with `std::make_shared()`:

```
GraphPtr graphPtr = std::make_shared<OrientedGraph>("GraphName");
```

If you know vertex counts at construction time, you can reserve memory in the constructor by passing `buffer_size` (bytes for the object) and optionally `chunk_size` (block size when the buffer is exhausted). Constructor signature:

```
OrientedGraph(const std::string &i_name = "", size_t buffer_size = DEFAULT_BUF, size_t chunk_size = CHUNK_SIZE);
```

Example: if the graph will have `m` inputs, `n` outputs, `k` gates, `f` constants, `l` sequential elements, and `r` subgraphs, pass:

```
size_t s = m * sizeof(GraphVertexInput) + n * sizeof(GraphVertexOutput) + k * sizeof(GraphVertexGates) +
  f * sizeof(GraphVertexConstant) + l * sizeof(GraphVertexSequental) + r * sizeof(GraphVertexSubgraph);
GraphPtr graph = std::make_shared<OrientedGraph>("", s);
```

See maximum vertex sizes in [`GraphMemory.hpp`](../../include/CircuitGenGraph/GraphMemory.hpp) (`MultiLinearAllocator`, `buf_size`).

### Reserving vertices by type

If you know how many vertices of a given type you need:

```
void reserve(VertexTypes i_type, size_t i_capacity)
```

### Reserving adjacency lists

If input/output counts are known:

```
void GraphVertexBase::reserveInConnections(size_t i_size)
void GraphVertexBase::reserveOutConnections(size_t i_size)
```

**Русский:** [MEMORY_MANAGEMENT.md](../ru/MEMORY_MANAGEMENT.md)
