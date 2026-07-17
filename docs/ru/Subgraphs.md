# Подграфы и иерархия

Иерархические схемы в CircuitGen Graph строятся из вложенных **`OrientedGraph`** и вершин типа **`GraphVertexSubGraph`**.

## Добавление подграфа

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
// outputs — выходные вершины-обертки экземпляра подграфа на parent
```

- `addSubGraph(GraphPtr i_subGraph, std::vector<VertexPtr> i_inputs)` регистрирует `child` в `d_subGraphs` родителя и создает вершину-подграф с привязкой входов.
- `getSubGraphs()` возвращает множество вложенных графов.

## Внешний Verilog-файл

У вершины подграфа можно указать путь к отдельному RTL-файлу:

```cpp
auto *sgVertex = /* GraphVertexSubGraph * */;
sgVertex->setVerilogPath("/path/to/child_module.v");
```

При экспорте в Verilog модуль может подключаться из файла; параметры модуля разбираются и хранятся в метаданных графа:

- `OrientedGraph::setVerilogParameter(name, value)`
- `getVerilogParameters()`
- `clearVerilogParameters()`

Поддерживаются `parameter` и `localparam` (см. `GraphVertexSubGraph` / `src/GraphVertexSubGraph.cpp`).

## Развертывание (unroll)

`toVerilog`, `toDOT` и **OpenABCD GraphML** могут работать с плоским представлением: метод **`unrollGraph()`** создает копию графа без иерархии подграфов (все уровни «вшиты» в один граф). Это важно для инструментов, не понимающих иерархию.

## Экспорт и GraphML

- **Classic GraphML** экспортирует каждый экземпляр подграфа как узел `subGraph` со вложенным `<graph>` и ребрами, связывающими порты родителя и подграфа. Для плоского экспорта без иерархии используйте `unrollGraph()`.
- **PseudoABCD** и **OpenABCD** вызывают `unrollGraph()` перед сериализацией, если в графе есть подграфы.

Подробнее: [GraphML.md](GraphML.md).

## Поиск вершин

`getVerticesByName(name, i_addSubGraphs)` и родственные методы принимают флаг поиска внутри подграфов (`i_addSubGraphs = true`).

## См. также

- [VerilogImport.md](VerilogImport.md) — импорт модулей в `Context::d_graphs`  
- [FUNCTIONALITY.md](FUNCTIONALITY.md)  
- Тесты: `test/` (сценарии `SubGraph`, `toVerilog`, `toDOT`)

**English:** [Subgraphs.md](../en/Subgraphs.md)
