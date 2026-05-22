# Импорт Verilog

Импорт RTL в `OrientedGraph` выполняется через **`GraphReader`** (колбэки [lorina](https://hub.mos.ru/circuitgen/lorina)) и статические методы **`OrientedGraph::readVerilog`**.

## API

```cpp
#include <CircuitGenGraph/GraphReader.hpp>
#include <CircuitGenGraph/OrientedGraph.hpp>

CG_Graph::Context context;
CG_Graph::OrientedGraph::readVerilog("/path/to/module.v", context);

// Все разобранные модули:
for (const auto &[name, graph] : context.d_graphs) { /* … */ }

// Или одноразовый вызов с возвратом контекста:
auto ctx = CG_Graph::OrientedGraph::readVerilog("/path/to/module.v");
```

### Класс `Context`

| Поле | Назначение |
|------|------------|
| `d_currentTopName` | Имя верхнего модуля при иерархическом разборе |
| `d_graphs` | Карта `имя_модуля` → `GraphPtr` уже построенных графов |
| `d_currentGraph` | Граф, собираемый в данный момент |
| `d_currentGraphNamesList` | Быстрый поиск вершин по имени в текущем графе |
| `d_numberOfVertices` | Подсказка для резервирования памяти |

Повторный разбор нескольких файлов с общим `Context` позволяет связать инстансы подмодулей (см. комментарии в `GraphReader.hpp`; поддержка модулей в разных файлах развивается).

## Поведение

1. Открывается файл; парсер ищет первое ключевое слово `module`.
2. Вызывается `lorina::read_verilog` с экземпляром `GraphReader`.
3. При `parse_error` выбрасывается `std::runtime_error`; при отсутствии файла — сообщение с путём.

`GraphReader` реализует колбэки: заголовок модуля, `input`/`output`/`wire`, `parameter`, `assign`, инстанцирование и др. (см. заголовок и `src/GraphReader.cpp`).

## Ограничения

- Поддерживается подмножество синтаксиса Verilog, которое покрывает lorina и тесты в `test/testModulesForReading/`.
- Сложная иерархия и SystemVerilog-конструкции могут не поддерживаться — проверяйте на своих модулях и дополняйте тесты.
- Для интеграции с **Generator** / **Parameters** обычно достаточно экспортировать граф обратно через `toVerilog`.

## Тесты

Юнит-тесты: `test/src/GraphReaderTests.cpp`, эталонные `.v` в `test/testModulesForReading/`.

```sh
ctest --preset=dev --output-on-failure
```

## Зависимость lorina

Подмодуль: `lib/lorina` (репозиторий [circuitgen/lorina](https://hub.mos.ru/circuitgen/lorina)). Обновление парсера — через bump подмодуля и пересборку Graph.

## См. также

- [FUNCTIONALITY.md](FUNCTIONALITY.md)  
- [Subgraphs.md](Subgraphs.md) — внешние `.v` у вершин подграфа  
- [Lorina FUNCTIONALITY](https://hub.mos.ru/circuitgen/lorina/-/blob/main/docs/ru/FUNCTIONALITY.md)

**English:** [VerilogImport.md](../en/VerilogImport.md)
