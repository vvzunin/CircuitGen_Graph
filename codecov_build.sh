#!/bin/bash

# Остановка выполнения при ошибке
set -e

# Переменные
BUILD_DIR="build"
COVERAGE_DIR="coverage_report"
COVERAGE_INFO="coverage.info"
BUILD_TYPE=${1:-Debug} # По умолчанию Debug

echo "*** Starting coverage generation ***"

# Удаляем старую сборку
if [ -d "$BUILD_DIR" ]; then
    echo "*** Removing old build directory ***"
    rm -rf "$BUILD_DIR"
fi

# Конфигурируем проект
echo "*** Configuring project with coverage enabled ***"
cmake --preset=dev -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE="$BUILD_TYPE" | tee cmake_config.log

# Сборка проекта
echo "*** Running build ***"
cmake --build --preset=dev -j $(nproc) | tee build.log

# Запуск тестов
echo "*** Running tests ***"
ctest --preset=dev --output-on-failure -j $(nproc) | tee tests.log

# Проверяем наличие lcov
if ! command -v lcov &> /dev/null; then
    echo "Error: lcov not found. Install it and try again."
    exit 1
fi

# Сбор покрытия
echo "*** Capturing coverage data ***"
lcov --capture --directory . --output-file "$COVERAGE_INFO" --ignore-errors mismatch,gcov | tee coverage.log

# Удаляем данные покрытия для системных файлов и ненужных папок
echo "*** Removing unnecessary files from coverage ***"
lcov --remove "$COVERAGE_INFO" '/usr/*' "$BUILD_DIR/*" 'test/*' 'lib/*' --output-file "$COVERAGE_INFO" | tee coverage_filter.log

# Генерация HTML-отчета
echo "*** Generating HTML report ***"
if [ -d "$COVERAGE_DIR" ]; then
    rm -rf "$COVERAGE_DIR"
fi
genhtml "$COVERAGE_INFO" --output-directory "$COVERAGE_DIR" | tee genhtml.log

# Открытие отчета
if command -v xdg-open &> /dev/null; then
    echo "*** Opening coverage report ***"
    xdg-open "$COVERAGE_DIR/index.html" || echo "Coverage report generated at $COVERAGE_DIR/index.html"
else
    echo "Coverage report generated at $COVERAGE_DIR/index.html"
fi

echo "*** Coverage report successfully generated! ***"
