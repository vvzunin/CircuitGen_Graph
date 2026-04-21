# Библиотека графа (CircuitGen Graph)

**Язык:** Русский | [English](README.en.md)
**Документация:** [Русский](docs/ru/README.md) | [English](docs/en/README.md)
**Версионирование (SemVer):** [русский](docs/ru/Versioning.md) | [English](docs/en/Versioning.md)
**Вклад:** [русский](docs/ru/CONTRIBUTING.md) | [English](docs/en/CONTRIBUTING.md)
**Запросы на слияние (MR):** [русский](docs/ru/MergeRequests.md) | [English](docs/en/MergeRequests.md)

<!--
![License: MIT](https://img.shields.io/github/license/vvzunin/CircuitGen_Graph)
![GitHub forks](https://img.shields.io/github/forks/vvzunin/CircuitGen_Graph)
![GitHub Repo stars](https://img.shields.io/github/stars/vvzunin/CircuitGen_Graph)
![GitHub watchers](https://img.shields.io/github/watchers/vvzunin/CircuitGen_Graph)

[![codecov](https://codecov.io/gh/vvzunin/CircuitGen_Graph/graph/badge.svg?token=U88U82QFX8)](https://codecov.io/gh/vvzunin/CircuitGen_Graph)
![GitHub Release](https://img.shields.io/github/v/release/vvzunin/CircuitGen_Graph)
![GitHub Release Date](https://img.shields.io/github/release-date/vvzunin/CircuitGen_Graph)

![GitHub commit activity](https://img.shields.io/github/commit-activity/m/vvzunin/CircuitGen_Graph)
![GitHub commits since latest release](https://img.shields.io/github/commits-since/vvzunin/CircuitGen_Graph/latest)
![GitHub last commit](https://img.shields.io/github/last-commit/vvzunin/CircuitGen_Graph)

![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/vvzunin/CircuitGen_Graph)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues-pr/vvzunin/CircuitGen_Graph)

![Alt](https://repobeats.axiom.co/api/embed/96480623d3ed662877d25bdc40716dbc9d20ec95.svg "Repobeats analytics image")
-->

[![pipeline status](https://hub.mos.ru/circuitgen/CircuitGen_Graph/badges/main/pipeline.svg)](https://hub.mos.ru/circuitgen/CircuitGen_Graph/-/commits/main)
[![coverage report](https://hub.mos.ru/circuitgen/CircuitGen_Graph/badges/main/coverage.svg)](https://hub.mos.ru/circuitgen/CircuitGen_Graph/-/commits/main)
[![Latest Release](https://hub.mos.ru/circuitgen/CircuitGen_Graph/-/badges/release.svg)](https://hub.mos.ru/circuitgen/CircuitGen_Graph/-/releases)

<font size="5">**Оглавление:**</font>
<a name="content_rus"></a>
- [Правила именования переменных](#hacking)
- [Сборка программы](#generator_build_rus)
- [Генерация тестбенчей](#testbench)
- [Лицензия](#license)

> Главный репозиторий проекта: https://hub.mos.ru/circuitgen/CircuitGen

Сборка и запуск производятся из операционной системы Linux.

Программа запускалась на Ubuntu 22.04 с использованием WSL (Windows Subsystem for Linux).

> В проект добавлена система логирования. Для корректной работы библиотеки необходимо инициализировать библиотеку командой:
> #include "easyloggingpp/easylogging++.h"
> INITIALIZE_EASYLOGGINGPP

## Установка зависимостей

Актуальные списки пакетов и доп. шаги (CMake с Kitware, **clang-format** с PyPI, **lcov** 2+ на Ubuntu 22.04, Doxygen и т.д.) собраны в скриптах каталога **[`scripts/setup/`](scripts/setup/)** — по одному файлу на ОС из CI-матрицы, например:

- [`install-deps-ubuntu-22.04.sh`](scripts/setup/install-deps-ubuntu-22.04.sh)
- [`install-deps-ubuntu-24.04.sh`](scripts/setup/install-deps-ubuntu-24.04.sh)
- [`install-deps-debian-13.sh`](scripts/setup/install-deps-debian-13.sh)
- [`install-deps-fedora-42.sh`](scripts/setup/install-deps-fedora-42.sh) / [`install-deps-fedora-43.sh`](scripts/setup/install-deps-fedora-43.sh)

Запуск от root: `sudo bash scripts/setup/install-deps-ubuntu-24.04.sh`.

Детали сборки и скриптов: [BUILDING.md](docs/ru/BUILDING.md), [SCRIPTS.md](docs/ru/SCRIPTS.md).

## Режим разработчика
<a name="hacking"></a>

Несколько советов, которые помогут Вам создать и протестировать этот проект в качестве разработчика и потенциального участника представлены [здесь](docs/ru/HACKING.md).

[&#8593; Contents](#content_rus)

## Сборка программы
<a name="generator_build_rus"></a>

Для сборки программы из корня репозитория (после `cp CMakeUserPresets.json.example CMakeUserPresets.json`):
```
bash scripts/dev/build-debug.sh
```

Подробная схема сборки описана [здесь](docs/ru/BUILDING.md).

[&#8593; Contents](#content_rus)

## Стиль кода
<a name="format"></a>

Предварительные действия перед работой с кодом в VS Code, а также информацию о стиле кода и локальных `.json` можно посмотреть [здесь](docs/ru/Format.md).

[&#8593; Contents](#content_rus)

## Генерация тестбенчей
<a name="testbench"></a>

Библиотека поддерживает автоматическую генерацию Verilog-тестбенчей для верификации схем. Кратко:

- пример программы: [`examples/example_testbench.cpp`](examples/example_testbench.cpp) (сборка вместе с остальными `examples/` при **`CircuitGenGraph_BUILD_EXAMPLES=ON`**, см. пресет **dev**);
- заголовок API: `#include <CircuitGenGraph/TestbenchGenerator.hpp>`;
- для симуляции в части тестов — **Icarus Verilog** (`iverilog` / `vvp`), пакеты: `sudo apt install iverilog` или `sudo dnf install iverilog`.

Пошаговая сборка примера, запуск `ctest` и сценарии с префиксом **`DISABLED_`** (Icarus) описаны в **[`docs/ru/BUILDING.md`](docs/ru/BUILDING.md#tests-and-icarus)**; английский вариант — [`docs/en/BUILDING.md`](docs/en/BUILDING.md#tests-and-icarus).

[&#8593; Contents](#content_rus)

## Сборка документации

Зависимости для Doxygen, LaTeX/PDF и вспомогательных инструментов входят в соответствующий [`scripts/setup/install-deps-*.sh`](scripts/setup/) для вашей ОС.

Сборка (HTML, LaTeX и PDF через `make` в каталоге LaTeX — см. `cmake/docs.cmake`):
```
cmake -DBUILD_MCSS_DOCS=ON --preset=dev
cmake --build build/dev --preset=dev -j $(nproc) --target docs
```

Готовый PDF: **`build/dev/docs/pdf/CircuitGenGraph.pdf`** (копируется из `build/dev/docs/latex/refman.pdf` целью `docs`).

Полный конвейер как в CI (в т.ч. PDF): **`bash scripts/ci/docs.sh`** — артефакты в **`build/docs/pdf/<язык>/`** (см. `docs/ru/SCRIPTS.md`).

# Лицензия
<a name="license"></a>

Смотрите файл с [лицензией](LICENSE).

[&#8593; Contents](#content_rus)
