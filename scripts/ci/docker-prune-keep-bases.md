# Руководство: `docker-prune-keep-bases.ps1`

Скрипт для хоста **Windows 11** с **Docker Desktop (WSL2)** и **GitLab Runner**: убирает с диска накопленные **небазовые** Docker-образы и при необходимости **сжимает** файл данных Docker `docker_data.vhdx`, чтобы освободилось место на томе Windows.

Файлы лежат в **`scripts/ci/`** этого репозитория; каталог **`scripts/ci`** синхронизируется между репозиториями **Parameters**, **Graph** и **Generator**.

---

## 1. Назначение

| Проблема | Что делает скрипт |
|----------|-------------------|
| Много CI-образов и тегов из частного registry на машине runner’а | Удаляет их через `docker rmi`, оставляя типовые базовые образы |
| После `docker rmi` в проводнике **не** растёт свободное место на `C:` | Опционально: `Optimize-VHD` для `docker_data.vhdx` |

---

## 2. Требования

**Этап очистки образов**

- **PowerShell 5.1+** (Windows 11 по умолчанию).
- **Docker CLI** в `PATH` (обычно после установки Docker Desktop).
- Запуск **не обязан** быть от администратора (достаточно прав пользователя, который может управлять Docker).

**Этап сжатия VHDX (`-CompactDockerDataVhdx`)**

- **PowerShell от имени администратора**.
- Компонент / модуль **Hyper-V** (команда `Optimize-VHD`), как при ручном сжатии диска.
- Установленный **Docker Desktop** с данными в стандартном пути WSL2 (см. ниже).

---

## 3. Два этапа работы

### 3.1. Очистка образов (включена по умолчанию)

1. **`docker image prune -f`** — удаление «висячих» слоёв (`<none>`), если не указан `-SkipDanglingPrune`.
2. **`docker images`** — для каждого образа с нормальным тегом проверяется поле **REPOSITORY**:
   - совпало с одним из правил «оставить» → образ **не трогаем**;
   - иначе → **`docker rmi repository:tag`**.
3. **`docker system df`** — краткая сводка использования данных внутри Docker.

**Важно:** если контейнер всё ещё использует образ, `docker rmi` для него завершится ошибкой — это ожидаемо. Остановите или удалите контейнеры, затем повторите при необходимости.

### 3.2. Сжатие `docker_data.vhdx` (опция `-CompactDockerDataVhdx`)

После удаления слоёв **внутри** виртуального диска появляется свободное место, но **размер файла `.vhdx` на диске Windows сам не уменьшается**. Сжатие:

1. Пытается корректно завершить **Docker Desktop** (`Docker Desktop.exe --quit`, ожидание до `DockerQuitWaitSeconds` секунд).
2. Выполняет **`wsl --shutdown`**, пауза `WslShutdownWaitSeconds` секунд.
3. Запускает **`Optimize-VHD -Path …\docker_data.vhdx -Mode Full`**.

После этого **Docker Desktop нужно запустить вручную**.

Файл по умолчанию:

`%LOCALAPPDATA%\Docker\wsl\disk\docker_data.vhdx`

Другой путь можно задать параметром **`-DockerDataVhdxPath`**.

---

## 4. Что остаётся на диске по умолчанию

Образ **сохраняется**, если **REPOSITORY** (как в выводе `docker images`) совпадает с **любым** из регулярных выражений:

| Шаблон (смысл) |
|----------------|
| `ubuntu`, `debian`, `fedora`, `docker` (в т.ч. образы с тегами **dind**), `alpine`, `busybox` — в короткой форме **или** как `docker.io/library/…` |
| `registry.gitlab.com/gitlab-org/release-cli` |

**Всё остальное** (в том числе ваш частный registry и CI-образы) по умолчанию **удаляется**.

Дополнительные исключения задаются параметром **`-ExtraKeepPattern`** (одна строка или массив regex).

---

## 5. Параметры (кратко)

| Параметр | Назначение |
|----------|------------|
| `-KeepRepositoryRegex` | Свой список regex вместо встроенного набора «базовых» образов |
| `-ExtraKeepPattern` | Дополнительные regex «не удалять» |
| `-SkipDanglingPrune` | Не вызывать `docker image prune -f` |
| `-SkipImagePrune` | Пропустить весь этап образов; имеет смысл только с `-CompactDockerDataVhdx` |
| `-CompactDockerDataVhdx` | Сжать `docker_data.vhdx` после (или вместо) очистки образов |
| `-DockerDataVhdxPath` | Полный путь к `.vhdx`, если не стандартный |
| `-DockerQuitWaitSeconds` | Таймаут ожидания выхода Docker (по умолчанию 90) |
| `-WslShutdownWaitSeconds` | Пауза после `wsl --shutdown` перед `Optimize-VHD` (по умолчанию 15) |
| `-WhatIf` | Показать, что было бы удалено / сжато, без выполнения деструктивных действий (где поддерживается) |
| `-Confirm` | Запрашивать подтверждение перед операциями, завязанными на `ShouldProcess` |

---

## 6. Типовые сценарии

Просмотр без удаления:

```powershell
cd <корень_репозитория>
.\scripts\ci\docker-prune-keep-bases.ps1 -WhatIf
```

Обычная очистка образов (без сжатия файла на диске Windows):

```powershell
.\scripts\ci\docker-prune-keep-bases.ps1
```

Очистка и сжатие VHDX (открыть **PowerShell от администратора**):

```powershell
.\scripts\ci\docker-prune-keep-bases.ps1 -CompactDockerDataVhdx
```

Не трогать образы Microsoft Container Registry:

```powershell
.\scripts\ci\docker-prune-keep-bases.ps1 -ExtraKeepPattern '^mcr\.microsoft\.com/'
```

Только сжать VHDX (образы уже чистили):

```powershell
.\scripts\ci\docker-prune-keep-bases.ps1 -SkipImagePrune -CompactDockerDataVhdx
```

Медленное закрытие Docker / WSL:

```powershell
.\scripts\ci\docker-prune-keep-bases.ps1 -CompactDockerDataVhdx -DockerQuitWaitSeconds 120 -WslShutdownWaitSeconds 25
```

---

## 7. Встроенная справка PowerShell

```powershell
Get-Help .\scripts\ci\docker-prune-keep-bases.ps1 -Full
Get-Help .\scripts\ci\docker-prune-keep-bases.ps1 -Examples
Get-Help .\scripts\ci\docker-prune-keep-bases.ps1 -Parameter CompactDockerDataVhdx
```

Текст совпадает с комментариями в начале `.ps1`.

**English:** [docker-prune-keep-bases.en.md](docker-prune-keep-bases.en.md)

---

## 8. Частые вопросы

**Долго выполняется удаление или `docker system df`**

При очень большом хранилище и слабом диске это возможно. Параллельно не запускайте тяжёлые операции Docker. При подозрении на зависание проверьте, не идёт ли ещё массовое `docker rmi`, и при необходимости остановите скрипт (`Ctrl+C`).

**Образы удалились, а место на `C:` не появилось**

Нужен этап **`-CompactDockerDataVhdx`** (админ + Hyper-V). Убедитесь, что сжимаете именно тот `.vhdx`, который реально разросся (часто это `...\Docker\wsl\disk\docker_data.vhdx`).

**`Optimize-VHD` не выполняется**

- Запуск от **администратора**.
- Установлен модуль **Hyper-V**.
- Docker и WSL полностью остановлены (при ошибке «файл занят» закройте Docker вручную, выполните `wsl --shutdown`, повторите).

**Нужно оставить свои базовые образы**

Добавьте **`-ExtraKeepPattern`** с regex по полному **REPOSITORY** из `docker images` (учитывайте registry и порт, например `vvzunin.me:5201/...`).

---

## 9. Автоматизация (по желанию)

- Разовый запуск по расписанию: **Планировщик заданий Windows** → `powershell.exe` с `-File` и полным путём к `docker-prune-keep-bases.ps1`.
- Для сценария **с `-CompactDockerDataVhdx`** у задания включите «Выполнять с наивысшими правами».

---

## 10. Осторожность

- Скрипт **удаляет локальные образы**, не совпавшие с правилами. Перед первым боевым запуском используйте **`-WhatIf`**.
- Этап **сжатия VHDX** останавливает **все дистрибутивы WSL** (`wsl --shutdown`), не только Docker.
- На общей машине согласуйте окно обслуживания: на время сжатия CI на этом runner’е лучше не планировать.

---

## 11. Связь с остальной документацией

- Конвейер GitLab и архитектура: [docs/ru/CI_PIPELINE.md](../../docs/ru/CI_PIPELINE.md)
- Справочник по всем скриптам `scripts/ci`: [docs/ru/CI_SCRIPTS.md](../../docs/ru/CI_SCRIPTS.md)
