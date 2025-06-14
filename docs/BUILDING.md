# Сборка с помощью CMake

### Сборка

Шаги для сборки в режиме выпуска с помощью генератора одной конфигурации, такого как Unix Makefiles:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Шаги для сборки в режиме выпуска с помощью генератора нескольких конфигураций, такого как Visual Studio:

```sh
cmake -S . -B build
cmake --build build --config Release
```

### Сборка с MSVC

Обратите внимание, что MSVC по умолчанию не соответствует стандартам, и вам необходимо передать некоторые флаги, чтобы он работал правильно. См. предустановку `flags-msvc` в файле [CMakePresets.json](../CMakePresets.json), где указаны флаги и переменная, которую нужно предоставить CMake во время настройки.

### Сборка на Apple Silicon

CMake правильно поддерживает сборку на Apple Silicon, начиная с версии 3.20.1. Убедитесь, что у вас установлена [последняя версия][1].

### Установка

Предварительным условием является то, что проект уже должен быть собран с использованием вышеуказанных команд.

Для запуска приведенных ниже команд требуется как минимум CMake 3.15, поскольку это версия, в которой был добавлен [Установить проект][2].

Вот команда для установки артефактов режима выпуска с помощью генератора одной конфигурации, такого как Unix Makefiles:

```sh
cmake --install build
```

Вот команда для установки артефактов режима выпуска с помощью генератора нескольких конфигураций, такого как Visual Studio:

```sh
cmake --install build --config Release
```

### Пакет CMake

Этот проект может быть экспортирован с помощью CMake командой [`find_package`][3]. 
В GIT_TAG необходимо подставить номер последней версии проекта:

* Package name: `CircuitGenGraph`
* Target name: `CircuitGenGraph::CircuitGenGraph`

Пример подгрузки библиотеки:
```cmake
FetchContent_Declare(
  CircuitGen_Graph
  GIT_REPOSITORY https://github.com/vvzunin/CircuitGen_Graph
  GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(CircuitGen_Graph)
```

Пример добавления библиотеки в проект:
```cmake
target_link_libraries(
    app PRIVATE
    CircuitGenGraph::CircuitGenGraph
)
```

### Дальнейшее использование
После подключения библиотеки доступны следующие типы данных:
* GraphVertexInput
* GraphVertexOutput
* GraphVertexGate
* GraphVertexConstant
* GraphVertexSubGraph
* OrientedGraph

Основным классом является граф (OrientedGraph). Для его подключения требуется указать хедер:

```
#include <CircuitGenGraph/OrientedGraph.hpp>
```

Для работы с вершинами графа вам потребуются следующий хедер:

```
#include <CircuitGenGraph/GraphVertex.hpp>
```

Различные полезные функции находятся в хедере в namespace `AuxMethodsGraph`:

```
#include <DefaultAuxilaryMethods.hpp>
```

Также есть экспортный хедер с перечислениями типов логических элементов, основных видов вершин и некоторыми другими; а также функциями для работы с ними:

```
#include <CircuitGenGraph/GraphUtils.hpp>
```
Подробное описание функций находится в автоматически сгенерированной [документации][4].

### Особенности установки

Для `CMAKE_INSTALL_INCLUDEDIR` задан путь, отличный от просто `include`, если проект настроен как проект верхнего уровня, чтобы избежать косвенного включения других библиотек при установке с общим префиксом. Просмотрите файл [install-rules.cmake](../cmake/install-rules.cmake) для получения полного набора правил установки.

[1]: https://cmake.org/download/
[2]: https://cmake.org/cmake/help/latest/manual/cmake.1.html#install-a-project
[3]: https://cmake.org/cmake/help/latest/command/find_package.html
[4]: https://vvzunin.github.io/CircuitGen_Graph/annotated.html
