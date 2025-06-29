#!/bin/bash
set -e

LATEX_DIR="$1"

if [ -z "$LATEX_DIR" ]; then
    echo "❌ Укажи путь до каталога с LaTeX (где лежит refman.tex)"
    exit 1
fi

if [ ! -f "${LATEX_DIR}/refman.tex" ]; then
    echo "❌ Файл refman.tex не найден в ${LATEX_DIR}"
    exit 1
fi

cd "$LATEX_DIR"

echo "📄 Компиляция PDF с помощью xelatex..."
xelatex -interaction=nonstopmode refman.tex >/dev/null || true
xelatex -interaction=nonstopmode refman.tex >/dev/null || true

if [ -f refman.pdf ]; then
    echo "✅ PDF успешно создан: ${LATEX_DIR}/refman.pdf"
else
    echo "❌ Ошибка: PDF не был создан"
fi

