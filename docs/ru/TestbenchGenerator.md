# Генерация тестбенчей

API **`TestbenchGenerator`** (`include/CircuitGenGraph/TestbenchGenerator.hpp`) строит Verilog testbench для проверки схем, представленных как `OrientedGraph`, и опционально запускает симуляцию.

## Основные типы

| Тип | Назначение |
|-----|------------|
| `TestbenchConfig` | `timescale`, период такта, сброс, VCD, таймаут |
| `TestVectorStrategy` | Стратегия стимулов (полный перебор, случайные векторы, …) |
| `VerificationResult` | Итоги прогона: passed/failed, векторы, вывод симулятора |
| `TestbenchGenerator` | Генерация `.v` bench и вызов внешнего симулятора |

Заголовок подключается после `OrientedGraph.hpp`. Инициализация логирования графа: `OrientedGraph::initLogging` (см. корневой README).

## Сценарии

- **Комбинационные** схемы: таблица истинности, сравнение ожидаемых и фактических выходов.
- **Последовательные** фрагменты: частичная поддержка (такт, сброс из `TestbenchConfig`); полный набор sequential-сценариев расширяется по мере развития API.

## Симуляция (Icarus Verilog)

Для автоматической верификации в тестах используется **Icarus** (`iverilog`, `vvp`). Пакет входит в `scripts/setup/install-deps-*.sh`.

Часть тестов GoogleTest помечена префиксом **`DISABLED_`**, чтобы обычный `ctest` не требовал симулятор:

```sh
./build/dev/test/CircuitGenGraph_tests \
  --gtest_filter='TestbenchGeneratorTests.DISABLED_IcarusVerificationAndGate' \
  --gtest_also_run_disabled_tests
```

Полный чек-лист: [BUILDING.md#tests-and-icarus](BUILDING.md#tests-and-icarus).

## Демо-программа

Исходник: `examples/example_testbench.cpp` (сборка при `CircuitGenGraph_BUILD_EXAMPLES=ON`, пресет **dev**). Запуск:

```sh
cmake --preset=dev
cmake --build --preset=dev -j "$(nproc)"
./build/dev/examples/example_testbench
```

## См. также

- [README.md](../../README.md#testbench) — краткий обзор  
- [Sequential.md](Sequential.md) — последовательностная логика в графе  
- [BUILDING.md](BUILDING.md) — пресеты и `ctest`

**English:** [TestbenchGenerator.md](../en/TestbenchGenerator.md)
