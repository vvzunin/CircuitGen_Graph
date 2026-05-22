# Documentation hub

The [repository root README](../README.md) is the project landing page (overview, build, links). This file is only the **documentation tree** entry point.

- [Русский — полное оглавление](ru/README.md)
- [English — full index](en/README.md)

Published API reference (HTML + PDF, RU/EN) after CI **`docs`** job on the default branch or Git tags:

- Portal: [https://vvzunin.me/docs/CircuitGen/](https://vvzunin.me/docs/CircuitGen/) — version selector per module when several channels exist (`main` + release tags).
- This module: `modules/graph/` with `versions/main/` (branch) and `versions/<tag>/` (tags); see [en/DEPLOY.md](en/DEPLOY.md) · [ru/DEPLOY.md](ru/DEPLOY.md).

CI and `scripts/ci` (kept in sync across Parameters / Graph / Generator):

- [CI pipeline (RU)](ru/CI_PIPELINE.md) · [CI pipeline (EN)](en/CI_PIPELINE.md)
- [CI scripts reference (RU)](ru/CI_SCRIPTS.md) · [CI scripts reference (EN)](en/CI_SCRIPTS.md) — NAS deploy ([en/DEPLOY.md](en/DEPLOY.md) · [ru/DEPLOY.md](ru/DEPLOY.md); CI_SCRIPTS §7); Windows runner (`docker-prune-keep-bases.ps1`, §8 / `docker-prune-runner-windows`)

CMake presets, CI-parity build commands, and **how to add new sources / `CMakeLists.txt` targets** (same structure in **en**/**ru** across all three repos):

- [BUILDING (EN)](en/BUILDING.md) · [BUILDING (RU)](ru/BUILDING.md)
- [FUNCTIONALITY (EN)](en/FUNCTIONALITY.md) · [FUNCTIONALITY (RU)](ru/FUNCTIONALITY.md) — repository role and capabilities

Graph-specific guides:

- Sequential: [RU](ru/Sequential.md) · [EN](en/Sequential.md)
- Memory management: [RU](ru/MEMORY_MANAGEMENT.md) · [EN](en/MEMORY_MANAGEMENT.md)
- GraphML: [RU](ru/GraphML.md) · [EN](en/GraphML.md)
- Verilog import: [RU](ru/VerilogImport.md) · [EN](en/VerilogImport.md)
- Subgraphs: [RU](ru/Subgraphs.md) · [EN](en/Subgraphs.md)
- Testbench: [RU](ru/TestbenchGenerator.md) · [EN](en/TestbenchGenerator.md)
- Changelog: [RU](ru/CHANGELOG.md) · [EN](en/CHANGELOG.md)
