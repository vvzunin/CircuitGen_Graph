# Экспорт GraphML

Библиотека **CircuitGenGraph** поддерживает три варианта GraphML для обмена с внешними инструментами и репозиторием **Generator**.

## Форматы

| Формат | Методы | Назначение |
|--------|--------|------------|
| **Classic** | `toGraphMLClassic()` | Человекочитаемые типы узлов (`input`, `and`, `sequential/ff`, …) |
| **PseudoABCD** | `toGraphMLPseudoABCD()` | Компактная схема с числовыми `node_type` (совместимость с ML-пайплайнами) |
| **OpenABCD** | `toGraphMLOpenABCD()` | Расширенная ABCD-схема: `node_id`, `node_type`, `num_inverted_predecessors`, `edge_type`; при наличии подграфов граф **разворачивается** (`unrollGraph`) |

Шаблоны XML: `src/GraphMLTemplates.hpp`. Запись в файл — через перегрузки с `std::ofstream` (добавляется комментарий с датой генерации).

## Classic GraphML

У каждого узла атрибут `type` (ключ `t`):

| Вершина | Значение `type` |
|---------|-----------------|
| Вход / выход | `input`, `output` |
| Константа | символ значения (`0`, `1`, …) |
| Вентиль | имя вентиля из `GraphUtils::parseGateToString` (например `and`, `nand`) |
| Последовательность | `sequential/ff` или `sequential/latch` |
| Подграф | вложенный `<graph>` внутри узла `subGraph`, плюс рёбра к портам родителя; для плоского экспорта без иерархии удобнее ABCD-форматы (`unrollGraph`) |

Ребра — направленные `<edge source="…" target="…"/>`. Подробнее о sequential: [Sequential.md](Sequential.md).

## PseudoABCD и OpenABCD

Общая таблица кодов вентилей (`gateToABCDType` в `GraphMLTemplates.hpp`):

| Вентиль | `node_type` |
|---------|-------------|
| AND | 2 |
| NAND | 10 |
| OR | 13 |
| NOR | 14 |
| NOT | 12 |
| BUF | 11 |
| XOR | 15 |
| XNOR | 16 |

Дополнительно:

| Вершина | `node_type` |
|---------|-------------|
| Вход (только OpenABCD) | 0 |
| Выход (только OpenABCD) | 1 |
| Константа | `100` + значение (`1000`, `1001`, …) |
| Триггер (ff) | 17 |
| Защелка (latch) | 18 |

При наличии подграфов **PseudoABCD** и **OpenABCD** сначала вызывают `unrollGraph()`. **OpenABCD** дополнительно пропускает узлы BUF/NOT при построении (инверсия учитывается в `num_inverted_predecessors` на ребрах). **PseudoABCD** использует упрощенные шаблоны узлов и ребер без `edge_type`.

## Связь с Generator

При сохранении схемы Generator может запросить один или несколько форматов (`getMakeGraphMLClassic()`, `getMakeGraphMLPseudoABCD()`, `getMakeGraphMLOpenABCD()`). Все три строятся из одного `OrientedGraph`.

## См. также

- [FUNCTIONALITY.md](FUNCTIONALITY.md) — обзор возможностей  
- [Subgraphs.md](Subgraphs.md) — иерархия и развертывание  
- [BUILDING.md](BUILDING.md) — сборка и каталог `examples/` (программы `example_export_graphml_*`)

**English:** [GraphML.md](../en/GraphML.md)
