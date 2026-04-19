# Справочник: `scripts/ci`

Текст **синхронизируется** между репозиториями Parameters, Graph и Generator. Поведение скриптов одно и то же; в job’ах GitLab подставляются переменные (`REPO_NAME`, `DOCKER_URL`, …) из `.gitlab-ci.yml`.

Общая картина конвейера: [CI_PIPELINE.md](CI_PIPELINE.md).

---

## 1. Запуск задач локально

| Скрипт | Назначение |
|--------|------------|
| **`run-task.sh`** | Одна задача: `bash scripts/ci/run-task.sh lint` (и т.д.). Переменные: `CI_RUNNER=local\|docker`, `CI_IMAGE_TAG`, `TARGET_OS`. |
| **`run-all.sh`** | Последовательный прогон основных CI-задач (как в типичном pipeline). |

Список задач для `run-task.sh` см. внутри скрипта (например: `lint`, `sanitize`, `static-analysis`, `coverage`, `tests`, `examples`, `docs`).

---

## 2. Сборка и публикация Docker-образов

| Скрипт | Назначение |
|--------|------------|
| **`docker-build-ci.sh`** | Сборка и push образа **CI** (`buildx`), аргументы из `docker-paths.sh` / переменных CI. |
| **`docker-build-dev.sh`** | Образ **dev** (на базе CI-образа). |
| **`docker-build-release.sh`** | Образ **release**. |
| **`docker-registry-login.sh`** | `docker login` в registry CI (переменные GitLab CI/CD). |
| **`docker-skip-if-unchanged.sh`** | `docker manifest inspect` + `docker-context-changed.sh`; exit 0 — пропустить сборку. Аргумент: `ci` \| `dev` \| `release`. |
| **`docker-context-changed.sh`** | Сравнение `git diff` по путям, влияющим на слой образа; аргумент: `ci` \| `dev` \| `release`. |
| **`docker-hub-proxy-image.sh`** | Переписывает короткую ссылку Docker Hub (`ubuntu:24.04`) в путь Harbor proxy (`REGISTRY_URL`/`DOCKER_HUB_PROXY_PROJECT`). |

---

## 3. Генерация и проверка `.gitlab-ci.yml`

| Скрипт | Назначение |
|--------|------------|
| **`generate-gitlab-os-matrix.sh`** | Генерация job’ов матрицы ОС между маркерами в `.gitlab-ci.yml`. Режимы: без аргументов (stdout), `--write`, `--check`. |

---

## 4. Проверки кода (основные entrypoint’ы)

| Скрипт | Назначение |
|--------|------------|
| **`lint.sh`** | Линт (формат, правила проекта). |
| **`static-analysis.sh`** | Статический анализ (например cppcheck). |
| **`sanitize.sh`** | Сборка/тесты со санитайзерами. |
| **`tests.sh`** | Запуск тестов (CTest и т.п.). |
| **`coverage.sh`** | Покрытие кода. |
| **`examples.sh`** | Сборка/запуск примеров. |
| **`docs.sh`** | Сборка документации в CI. |

---

## 5. Образы ОС и полные проверки

| Скрипт | Назначение |
|--------|------------|
| **`os-image-build-push.sh`** | Сборка «чистого» образа ОС и push в registry (сценарии os-check). |
| **`os-full-check.sh`** | Полная проверка на образе ОС (install-deps, прогон проверок). |

---

## 6. Прочее

| Скрипт | Назначение |
|--------|------------|
| **`create-gitlab-release.sh`** | Создание GitLab Release (стадия **release**, теги). |
| **`apk-install-with-retry.sh`** | Установка пакетов **apk** в Alpine с повторами (вспомогательный shell для `before_script`). |

---

## 7. Windows: обслуживание диска runner’а

| Файл | Назначение |
|------|------------|
| **`scripts/ci/docker-prune-keep-bases.ps1`** | PowerShell: удалить локальные образы, кроме базовых; опционально **сжать** `docker_data.vhdx` (`Optimize-VHD`). |
| **[docker-prune-keep-bases.md](docker-prune-keep-bases.md)** | Руководство (в каталоге `docs/ru`). |
| **[docker-prune-keep-bases.md (EN)](../en/docker-prune-keep-bases.md)** | Руководство на английском (в каталоге `docs/en`). |

Встроенная справка: `Get-Help .\scripts\ci\docker-prune-keep-bases.ps1 -Full`.

---

## 8. Связанные каталоги

| Каталог | Роль |
|---------|------|
| `scripts/config` | `supported-os.sh` — матрица ОС для CI. |
| `scripts/docker` | `docker-paths.sh` и вспомогательные скрипты путей/registry. |
| `scripts/setup` | Установка зависимостей по ОС (используются Dockerfile’ы и os-check). |
| `scripts/dev`, `scripts/release`, `scripts/docs` | Не entrypoint’ы GitLab CI из `.gitlab-ci.yml`, но связаны со сборкой и релизом; см. [SCRIPTS.md](SCRIPTS.md). |
