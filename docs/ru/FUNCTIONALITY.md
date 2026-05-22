# CircuitGen Graph — описание функционала

**Репозиторий:** [CircuitGen_Graph](https://hub.mos.ru/circuitgen/CircuitGen_Graph)  
**CMake-проект:** `CircuitGenGraph`  
**Роль в экосистеме CircuitGen:** базовая C++-библиотека представления цифровых схем в виде ориентированного графа и экспорта/импорта в обменные форматы.

## Назначение

Graph — это ядро модели данных для генерации и анализа цифровых схем. Библиотека описывает схему как **ориентированный граф** (`OrientedGraph`): вершины — элементы схемы (входы, выходы, константы, логические вентили, последовательностные ячейки, подграфы), ребра — связи между ними. На этой модели строятся генераторы из репозитория **Generator**, а также внешние инструменты оптимизации и физического синтеза из **Parameters**.

## Основные возможности

### Модель графа

- Создание и изменение графа: добавление вершин, связей, иерархических **подграфов** (`subGraph`).
- Классификация вершин по типам (`VertexTypes`): вход/выход, константа, вентиль (`Gates`), последовательностная логика, шина, подграф.
- Поддержка **комбинационной** и **последовательной** логики (триггеры, защелки; см. [Sequential.md](Sequential.md)).
- Обновление уровней (топологическая глубина), подсчет вентилей и ребер, хеширование структуры графа.
- Управление памятью и контекстом через `GraphMemory`, вспомогательные утилиты в `GraphUtils`.

### Экспорт и визуализация

- **Verilog** — генерация RTL-описания схемы и иерархических модулей (`toVerilog`).
- **DOT** — вывод для Graphviz (`toDOT`).
- **GraphML** — обмен с внешними редакторами графов (используется в связке с Generator).
- Параметры модулей Verilog для подграфов: разбор и сохранение `parameter` / `localparam` при привязке внешнего Verilog-файла к вершине подграфа.

### Импорт

- **Чтение Verilog** в граф через парсер **lorina** (`GraphReader`, подмодуль `lib/lorina`).
- Контекст разбора нескольких модулей (`Context`) для иерархических описаний.
- Подробнее: репозиторий [Lorina](https://hub.mos.ru/circuitgen/lorina), [docs/ru/FUNCTIONALITY.md](https://hub.mos.ru/circuitgen/lorina/-/blob/main/docs/ru/FUNCTIONALITY.md).

### Симуляция на уровне графа

- Таблицы истинности для логических операций (`enums.hpp`, `Gates`).
- Обновление значений на вершинах для проверки поведения комбинационных фрагментов.

### Генерация тестбенчей

- API `TestbenchGenerator` — автоматическое построение Verilog testbench для верификации схем (комбинационные и частично последовательные сценарии).
- Примеры и тесты с **Icarus Verilog** (`iverilog` / `vvp`).

### Логирование

- Макросы `CG_LOG_*` поверх **easylogging++** (или заглушки при отключенном логировании).
- Инициализация через `OrientedGraph::initLogging`.

## Публичный API (ключевые заголовки)

| Заголовок | Назначение |
|-----------|------------|
| `OrientedGraph.hpp` | Главный класс графа, экспорт, иерархия |
| `GraphVertex*.hpp` | Типы вершин (вход, выход, вентиль, sequential, subgraph, …) |
| `GraphReader.hpp` | Импорт Verilog |
| `TestbenchGenerator.hpp` | Генерация testbench |
| `GraphUtils.hpp`, `enums.hpp` | Утилиты, перечисления, таблицы истинности |
| `Logging.hpp` | Логирование |

## Сборка и артефакты

- CMake 3.26+, C++17, пресеты `dev`, `release-ci` и др.
- Статическая/разделяемая библиотека `CircuitGenGraph`, опционально примеры (`examples/`) и тесты (`test/`).
- Документация: Doxygen (HTML/PDF, ru/en), публикация на Synology NAS из CI (`scripts/docs/deploy-synology.sh`).

## Зависимости

- **lorina** ([circuitgen/lorina](https://hub.mos.ru/circuitgen/lorina), подмодуль `lib/lorina`) — разбор Verilog; Graph использует только Verilog-reader.
- **easylogging++** — журналирование.
- Для документации и CI: Doxygen, LaTeX, Ninja, clang-format, lcov и др. (см. `scripts/setup/install-deps-*.sh`).

## Связь с другими репозиториями

| Репозиторий | Связь |
|-------------|--------|
| **Generator** | Создает `std::shared_ptr<OrientedGraph>` разными алгоритмами; сохраняет Verilog/GraphML |
| **Parameters** | Принимает сгенерированный Verilog для Yosys/ABC/OpenLane |
| **CircuitGen** (главный) | Координирует общий workflow проекта |

## Кому полезен этот репозиторий

- Разработчикам генераторов схем, которым нужна единая структура данных и экспорт в Verilog.
- Исследователям, работающим с иерархическими RTL, импортом модулей и автоматической верификацией (testbench).
- Интеграторам EDA-цепочек, подключающим CircuitGen к симуляторам и синтезаторам.

## См. также

- [README.md](../../README.md) — быстрый старт  
- [BUILDING.md](BUILDING.md) — пресеты и CI parity  
- [HACKING.md](HACKING.md) — workflow разработчика  
- [Sequential.md](Sequential.md) — последовательностная логика
- [GraphML.md](GraphML.md) — экспорт и ограничения GraphML
- [VerilogImport.md](VerilogImport.md) — импорт Verilog
- [Subgraphs.md](Subgraphs.md) — работа с подграфами
- [TestbenchGenerator.md](TestbenchGenerator.md) — генератор testbench
- [MEMORY_MANAGEMENT.md](MEMORY_MANAGEMENT.md) — управление памятью
- English version: [FUNCTIONALITY.md](../en/FUNCTIONALITY.md)
