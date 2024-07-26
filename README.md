# Генератор комбинационных схем

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

<font size="5">**Оглавление:**</font>
<a name="content_rus"></a> 
- [Правила именования переменных](#hacking)
- [Сборка программы](#generator_build_rus)
- [Лицензия](#license)

> [!IMPORTANT]  
> Главный репозиторий проекта: https://github.com/vvzunin/CircuitGen

Сборка и запуск производятся из операционной системы Linux.

Программа запускалась на Ubuntu 22.04 с использованием WSL (Windows Subsystem for Linux).

> [!IMPORTANT]  
> В проект добавлена система логирования. Для корректной работы библиотеки необходимо инициализировать библиотеку командой:
> #include "easylogging++.h"
> INITIALIZE_EASYLOGGINGPP

## Режим разработчика
<a name="hacking"></a> 

Несколько советов, которые помогут Вам создать и протестировать этот проект в качестве разработчика и потенциального участника представлены [здесь](HACKING.md).

[&#8593; Contents](#content_rus)

## Сборка программы
<a name="generator_build_rus"></a> 

Для сборки программы необходимо выполнить следующие команды из начальной директории:
```
chmod +x buildGraph.sh
./buildGraph.sh
```

Подробная схема сборка описана [здесь](BUILDING.md).

[&#8593; Contents](#content_rus)

# Лицензия
<a name="license"></a>

Смотрите файл с [лицензией](LICENSE.md).

[&#8593; Contents](#content_rus)
