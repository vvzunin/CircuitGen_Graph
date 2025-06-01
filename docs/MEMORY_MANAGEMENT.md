# О возможностях системы менеджмента памяти

### Инициализация графа

В общем случае граф следует создавать с помощью std::make_shared():
```
GraphPtr graphPtr = std::make_shared<OrientedGraph>("GraphName");
```
Если количество вершин известно в момент создания графа, можно выделить 
память под все вершины при его инициализации: для этого при вызове конструтора
нужно передать аргументы `buffer_size` - объем памяти под объект в байтах и,
при необходимости, `chunk_size` - размер блока памяти, который будет выделяться 
при израсходовании имеющегося объема. Сигнатура конструктора OrientedGraph:
```
 OrientedGraph(const std::string &i_name = "", size_t buffer_size = DEFAULT_BUF, size_t chunk_size = CHUNK_SIZE);
```
Например, если известно, что в графе будут использованы m входов, n выходов,
k логических элементов, f констант, l последовательностных элементов, r подграфов,
то в buffer_size нужно передать:
```
size_t s = m * sizeof(GraphVertexInput) + n * sizeof(GraphVertexOutput) + k * sizeof(GraphVertexGates) + 
f * sizeof(GraphVertexConstant) + l * sizeof(GraphVertexSequental) + r * sizeof(GraphVertexSubgraph);
GraphPtr graph = std::make_shared<OrientedGraph>("", s);
```
Максимальный размер одной вершины можно посмотреть в [GraphMemory.hpp](../include/CircuitGenGraph/GraphMemory.hpp)
в конструкторе класса MultiLinearAllocator в docstring к buf_size.
### Выделение памяти под все вершины определенного типа
Если количество вершин вершин некоторого типа задано -
можно выделить память для них с помощью метода:
```
void reserve(VertexTypes i_type, size_t i_capacity)
```
где i_capacity запланированное количество вершин.
### Выделение памяти для списков смежности в вершинах
Если количество входов или выходов вершины определено, можно
зарезервировать память под них соответственно с помощью методов:
```
void GraphVertexBase::reserveInConnections(size_t i_size)

void GraphVertexBase::reserveOutConnections(size_t i_size)
```
где i_size - количество вершин, которые будут добавлены.