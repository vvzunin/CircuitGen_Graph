# Генератор комбинационных схем
<!--
![License: MIT](https://img.shields.io/github/license/vvzunin/CircuitGen_Graph)
![GitHub forks](https://img.shields.io/github/forks/vvzunin/CircuitGen_Graph)
![GitHub Repo stars](https://img.shields.io/github/stars/vvzunin/CircuitGen_Graph)
![GitHub watchers](https://img.shields.io/github/watchers/vvzunin/CircuitGen_Graph)

![GitHub CI](https://github.com/vvzunin/CircuitGen_Graph/actions/workflows/ci.yml/badge.svg)
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
- [Лицензия](#license)

> Главный репозиторий проекта: https://hub.mos.ru/circuitgen/CircuitGen

Сборка и запуск производятся из операционной системы Linux.

Программа запускалась на Ubuntu 22.04 с использованием WSL (Windows Subsystem for Linux).

> В проект добавлена система логирования. Для корректной работы библиотеки необходимо инициализировать библиотеку командой:
> #include "easyloggingpp/easylogging++.h"
> INITIALIZE_EASYLOGGINGPP

## Установка необходимых пакетов

### Установка пакетов

#### С помощью apt
```
sudo apt install clang clang-tidy clang-format-15 doxygen g++ gcc make openssl cmake lcov ninja-build
```

#### С помощью dnf (для rpm ветки)
```
sudo dnf install clang clang-tools-extra doxygen g++ gcc make openssl cmake lcov ninja-build
```
Так как в проекте используется `clang-format-15`, предлагается два возможных решения. 
1) Так как критических отличий между различными версиями clang нет, 
допустимо просто создать link с именем `clang-format-15`:  
```
sudo link /usr/bin/clang-format /usr/bin/clang-format-15
```
2) Допустимой является установка `clang-format-15` посредством `pip`. 
Отметим, что в данном случае создание link может быть сопряжено с некоторыми трудностями.
```
sudo dnf install python python-pip
```
Далее требуется скачать конкретную версию clang-format.
```
pip install clang-format==15.0.7
```

Для проверки корректности кода на этапе компиляции используется clang-tidy. В случае, если clang-tidy 
не будет найден, в терминал выведется соответствующее сообщение и компиляция прервется. 

### Установка cmake 3.28.1
В случае проблем при установке cmake, попробуйте данный способ:
```
sudo apt install tar wget
cd ~/
wget https://cmake.org/files/v3.28/cmake-3.28.1.tar.gz
tar xzf cmake-3.28.1.tar.gz
rm -rf cmake-3.28.1.tar.gz
cd cmake-3.28.1
./bootstrap
make -j$(nproc)
sudo make install
cd ..
sudo rm -rf cmake-3.28.1
```

## Режим разработчика
<a name="hacking"></a> 

Несколько советов, которые помогут Вам создать и протестировать этот проект в качестве разработчика и потенциального участника представлены [здесь](/docs/HACKING.md).

[&#8593; Contents](#content_rus)

## Сборка программы
<a name="generator_build_rus"></a> 

Для сборки программы необходимо выполнить следующие команды из начальной директории:
```
chmod +x buildGraph.sh
./buildGraph.sh
```

Подробная схема сборка описана [здесь](/docs/BUILDING.md).

[&#8593; Contents](#content_rus)

## Стиль кода
<a name="format"></a>

Предварительные действия перед работой с кодом в VsCode, а также некоторую информацию касательно стиля кода и локальных файлов .json можно посмотреть [здесь](/docs/FORMAT.md).

[&#8593; Contents](#content_rus)

# Лицензия
<a name="license"></a>

Смотрите файл с [лицензией](LICENSE).

[&#8593; Contents](#content_rus)
