# CircuitGen Graph library

**Language:** [Русский](README.md) | English  
**Documentation:** [Russian](docs/ru/README.md) | [English](docs/en/README.md)  
**Versioning (SemVer):** [Russian](docs/ru/Versioning.md) | [English](docs/en/Versioning.md)  
**Contributing:** [Russian](docs/ru/CONTRIBUTING.md) | [English](docs/en/CONTRIBUTING.md)  
**Merge requests:** [Russian](docs/ru/MergeRequests.md) | [English](docs/en/MergeRequests.md)

[![pipeline status](https://hub.mos.ru/circuitgen/CircuitGen_Graph/badges/main/pipeline.svg)](https://hub.mos.ru/circuitgen/CircuitGen_Graph/-/commits/main)
[![coverage report](https://hub.mos.ru/circuitgen/CircuitGen_Graph/badges/main/coverage.svg)](https://hub.mos.ru/circuitgen/CircuitGen_Graph/-/commits/main)

## Overview

This repository provides **CircuitGenGraph** — an oriented graph library used by the CircuitGen toolchain for representing and manipulating circuit structures. Build with CMake presets; see [BUILDING.md](docs/en/BUILDING.md) and [HACKING.md](docs/en/HACKING.md).

### Quick start (local)

```sh
cp CMakeUserPresets.json.example CMakeUserPresets.json
cmake --preset=dev
cmake --build --preset=dev
ctest --preset=dev
```

Docker image paths for local CI-like builds use the `circuitgen/graph/...` prefix (see `scripts/docker/docker-paths.sh`).

## Testbench generation

The library can emit Verilog testbenches for verification. See **[docs/en/BUILDING.md](docs/en/BUILDING.md#tests-and-icarus)** (Russian: [`docs/ru/BUILDING.md`](docs/ru/BUILDING.md#tests-and-icarus)) for the **`example_testbench`** walkthrough, `CTest` / GoogleTest usage, and optional **Icarus Verilog** (`iverilog`) setup.

## Installing dependencies

Use the per-OS scripts in **[scripts/setup/](scripts/setup/)** (same as CI): e.g. [`install-deps-ubuntu-24.04.sh`](scripts/setup/install-deps-ubuntu-24.04.sh), [`install-deps-debian-13.sh`](scripts/setup/install-deps-debian-13.sh), Fedora variants — run as root: `sudo bash scripts/setup/install-deps-ubuntu-24.04.sh`.

Details: [BUILDING.md](docs/en/BUILDING.md), [SCRIPTS.md](docs/en/SCRIPTS.md).

## License

See [LICENSE](LICENSE).
