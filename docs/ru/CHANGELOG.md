# Журнал изменений

Все заметные изменения **CircuitGen Graph** документируются в этом файле.

Формат основан на [Keep a Changelog](https://keepachangelog.com/ru/1.1.0/); версии соответствуют [Semantic Versioning](https://semver.org/lang/ru/) — см. [Versioning.md](Versioning.md).

**English:** [CHANGELOG.md](../en/CHANGELOG.md)

## [Unreleased]

### Added

- Импорт Verilog в граф (`GraphReader`, `OrientedGraph::readVerilog`, подмодуль **lorina**).
- Внешний Verilog-файл для вершин подграфа; разбор `parameter` / `localparam`.
- Публикация документации на Synology NAS из CI: общий портал, `manifest.json` схема v2, деревья версий (`versions/main/`, `versions/<tag>/`).
- Набор скриптов `scripts/docs/` (`versions-index.sh`, `manifest-merge.sh`, `modules-registry.json`, UI портала); `scripts/ci/test_deploy_mock.sh`.
- Тематические руководства: GraphML, импорт Verilog, подграфы, TestbenchGenerator (ru/en).
- Оглавления документации: `FUNCTIONALITY`, `Sequential`, `MEMORY_MANAGEMENT`, новые гайды.
- Обзор функционала в `docs/ru|en/FUNCTIONALITY.md`.
- Шины и `oneBitVerilog` для вентилей; рефакторинг sequential.

### Changed

- Единое логирование через макросы `CG_LOG_*` и **easylogging++**.
- Полный перевод `docs/en/Sequential.md`; уточнен порядок сигналов для `affre`.
- Устаревшие `docs/HACKING.md` и `docs/BUILDING.md` заменены редиректами на `docs/ru|en/`.
- Журнал изменений перенесен в `docs/ru|en/CHANGELOG.md`.

### Fixed

- Вывод `parameter` / `localparam` при генерации Verilog иерархических модулей.
- CI/docs: деплой на NAS (Auth v3, учётные данные, пути DSM); слияние manifest при параллельном CI модулей; переключение языка портала без дублирования карточек; `versions.json` из всех каналов в стадии.
- Тесты testbench: уникальные временные файлы; покрытие и загрузка **fmt**.
- Документация GraphML/Subgraphs: фактическое поведение Classic GraphML и `unrollGraph()`.

---

## [1.6.0] - 2026-04-24

### Added

- Генератор Verilog testbench с эталонным сравнением и тестами (Icarus).
- Примеры использования библиотеки в `examples/`.
- Двуязычная Doxygen-документация (EN/RU).
- Метаданные авторства (`AUTHORS`, теги `@author`).

### Changed

- Крупный рефакторинг CMake, GitLab CI (мульти-ОС), скриптов сборки и `docs/`.
- Синхронизация `BUILDING.md` с репозиториями Generator и Parameters.
- Усиление надежности CI: кэш, повторное использование Docker-образов, таймауты.

### Fixed

- Сериализация sequential-вершин в GraphML и DOT.
- Видимость методов `GraphVertexSequential` при сборке с LTO.
- Сборка PDF-документации (LaTeX, Fedora, libclang).
- Предупреждения `update_level`, форматирование, мелкие ошибки CI.

---

## [1.5.2] - 2025-05-13

### Added

- Новая версия API подграфов; удаление неиспользуемых вершин.
- Расширенные тесты.

### Changed

- Обновления Dockerfile.dev и `.gitlab-ci.yml`.
- Документация.

### Fixed

- Ошибки в тестах и хешировании графа; линтер.

---

## [1.5.1] - 2025-05-02

### Added

- Расширение `updateValue` для вершин.
- Предупреждения при неверном числе входов у вентилей.

### Changed

- Сборка Doxygen из исходников в Docker; обновление `Doxyfile.in`.
- README и CI.

### Fixed

- HOTFIX для последовательностной логики.
- Предупреждения clang; тесты триггеров.

---

## [1.5.0] - 2025-04-18

### Added

- `Dockerfile.ci` для CI-сборки.
- Новые тесты.

### Changed

- Обновление GitLab CI и версии проекта.

### Fixed

- Орфография, линтер.

---

## [1.4.2] - 2025-04-17

### Fixed

- Линтер; удаление отладочного `printf`.

### Changed

- Обновление Dockerfile.

---

## [1.4.1] - 2025-04-10

### Added

- Резервирование памяти для графа, вершин и ребер (`reserve`, `reserveInConnections` / `reserveOutConnections`).
- Метод `getVerticesByLevel`.
- Миграция на расширенную систему тестов; `Dockerfile`.

### Fixed

- Размер вершин; CMake; clang-format.

---

## [1.4.0] - 2025-03-19

### Added

- Поддержка последовательностных вершин (`GraphVertexSequential`) и экспорт в Verilog.
- Интеграция **fmt**; тесты `toVerilog` для sequential.

### Changed

- Пространства имен и структура CMake.

### Fixed

- Множественные исправления линтера и тестов.

---

## [1.3.1] - 2025-01-16

### Added

- Базовый конвейер GitLab CI (`.gitlab-ci.yml`).
- Doxygen-комментарии для классов.

### Changed

- Оптимизации компиляции библиотеки; экспорт DOT.
- Виртуальные деструкторы; `conf.py` / `conf.py.in`.

### Fixed

- Орфография, линтер.

---

## [1.3.0] - 2024-12-03

### Added

- Интеграция clang-tidy в сборку.
- Обновление расчета уровней и хеша графа.

### Changed

- Форматирование кода; документация Markdown.
- Эксперименты с PMR (частично откат).

### Fixed

- Множественные исправления графа, тестов и CI.

---

## [1.2.0] - 2024-08-23

### Added

- Экспорт **DOT** (`toDOT`) для всех типов вершин.
- `toVerilog` для тестирования; сообщения об автогенерируемых файлах.

### Changed

- Логирование (easylogging++).

### Fixed

- `toDOT`, `alreadyParsed`; тесты.

---

## [1.1.3] - 2024-08-02

### Changed

- Порядок вентилей для GraphML OpenABCD / PseudoABCD.
- Индексы `VertexTypes`, `updateLevel()`.

### Fixed

- `getMaxLevel`; лишние логи.

---

## [1.1.2] - 2024-08-02

### Added

- Файл [LICENSE](../../LICENSE).

### Changed

- Переход на системную **easylogging++**; workflow и CMake.

### Fixed

- Segmentation fault в `toVerilog` для выходов.

---

## [1.1.1] - 2024-08-01

### Changed

- Порядок `source`/`target` в `toGraphMLOpenABCD`.
- CMakeLists, логирование.

---

## [1.1.0] - 2024-07-26

### Added

- Базовое логирование для классов графа.

### Fixed

- Sanitize CI, override `toVerilog`, easylogging, workflow, линтер.

---

## [1.0.0] - 2024-07-01

### Added

- Первая публикуемая версия библиотеки **CircuitGenGraph**: ориентированный граф, вершины, CMake-сборка.
- Примеры и базовые GitHub CI-файлы.

### Changed

- Упрощение структуры проекта и путей заголовков.

### Fixed

- CMake, тесты, README.

---

## Как обновлять

При релизе `vX.Y.Z`:

1. Перенесите пункты из **Unreleased** в `## [X.Y.Z] - YYYY-MM-DD`.
2. Создайте тег: `git tag vX.Y.Z`.
3. Подсказка версии: `bash scripts/release/suggest-next-version.sh`.

Обновляйте **оба** файла: `docs/ru/CHANGELOG.md` и `docs/en/CHANGELOG.md`.
