# CircuitGen Graph library

**Language:** [Русский](README.md) | English  
**Documentation:** [Russian](docs/ru/README.md) | [English](docs/en/README.md)  
**Functionality:** [Russian](docs/ru/FUNCTIONALITY.md) | [English](docs/en/FUNCTIONALITY.md)  
**Versioning (SemVer):** [Russian](docs/ru/Versioning.md) | [English](docs/en/Versioning.md)  
**Changelog:** [Russian](docs/ru/CHANGELOG.md) | [English](docs/en/CHANGELOG.md)  
**Contributing:** [Russian](docs/ru/CONTRIBUTING.md) | [English](docs/en/CONTRIBUTING.md)  
**Merge requests:** [Russian](docs/ru/MergeRequests.md) | [English](docs/en/MergeRequests.md)

[![pipeline status](https://hub.mos.ru/circuitgen/CircuitGen_Graph/badges/main/pipeline.svg)](https://hub.mos.ru/circuitgen/CircuitGen_Graph/-/commits/main)
[![coverage report](https://hub.mos.ru/circuitgen/CircuitGen_Graph/badges/main/coverage.svg)](https://hub.mos.ru/circuitgen/CircuitGen_Graph/-/commits/main)

## Overview

This repository provides **CircuitGenGraph** — an oriented graph library used by the CircuitGen toolchain for representing and manipulating circuit structures. Build with CMake presets; see [BUILDING.md](docs/en/BUILDING.md) and [HACKING.md](docs/en/HACKING.md). Topic guides: [GraphML](docs/en/GraphML.md), [Verilog import](docs/en/VerilogImport.md), [Subgraphs](docs/en/Subgraphs.md), [Sequential](docs/en/Sequential.md), [Testbench](docs/en/TestbenchGenerator.md), [Memory](docs/en/MEMORY_MANAGEMENT.md).

### Quick start (local)

```sh
cp CMakeUserPresets.json.example CMakeUserPresets.json
cmake --preset=dev
cmake --build --preset=dev
ctest --preset=dev
```

Docker image paths for local CI-like builds use the `circuitgen/graph/...` prefix (see `scripts/docker/docker-paths.sh`).

## Testbench generation

The library can emit Verilog testbenches for verification. See **[docs/en/BUILDING.md](docs/en/BUILDING.md#tests-and-icarus)** (Russian: [`docs/ru/BUILDING.md`](docs/ru/BUILDING.md#tests-and-icarus)) and **[TestbenchGenerator.md](docs/en/TestbenchGenerator.md)** for the API, **`example_testbench`**, `CTest` / GoogleTest, and optional **Icarus Verilog** (`iverilog`).

## Installing dependencies

Use the per-OS scripts in **[scripts/setup/](scripts/setup/)** (same as CI): e.g. [`install-deps-ubuntu-24.04.sh`](scripts/setup/install-deps-ubuntu-24.04.sh), [`install-deps-debian-13.sh`](scripts/setup/install-deps-debian-13.sh), Fedora variants — run as root: `sudo bash scripts/setup/install-deps-ubuntu-24.04.sh`.

Details: [BUILDING.md](docs/en/BUILDING.md), [SCRIPTS.md](docs/en/SCRIPTS.md).

## License

See [LICENSE](LICENSE).
