# Documentation hub

The [repository root README](../README.md) is the project landing page (overview, build, links). This file is only the **documentation tree** entry point.

- [Русский — полное оглавление](ru/README.md)
- [English — full index](en/README.md)

CI and `scripts/ci` (kept in sync across Parameters / Graph / Generator):

- [CI pipeline (RU)](ru/CI_PIPELINE.md) · [CI pipeline (EN)](en/CI_PIPELINE.md)
- [CI scripts reference (RU)](ru/CI_SCRIPTS.md) · [CI scripts reference (EN)](en/CI_SCRIPTS.md) — including Windows runner maintenance (`docker-prune-keep-bases.ps1`, §7 / anchor `docker-prune-runner-windows`)

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
