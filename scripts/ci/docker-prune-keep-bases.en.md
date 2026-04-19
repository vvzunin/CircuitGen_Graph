# Manual: `docker-prune-keep-bases.ps1`

PowerShell script for **Windows 11** hosts with **Docker Desktop (WSL2)** and **GitLab Runner**: removes accumulated **non-base** Docker images and optionally **compacts** the Docker data file `docker_data.vhdx` so **free space on the Windows volume** increases.

The script and manuals live under **`scripts/ci/`** in this repository. The **`scripts/ci`** tree is **synchronized** across the **Parameters**, **Graph**, and **Generator** repositories.

**Russian:** [docker-prune-keep-bases.md](docker-prune-keep-bases.md)

---

## 1. Purpose

| Problem | What the script does |
|---------|----------------------|
| Many CI images and tags from a private registry on the runner host | Removes them with `docker rmi`, keeping common base images |
| After `docker rmi`, **no** extra free space appears on `C:` in Explorer | Optionally runs `Optimize-VHD` on `docker_data.vhdx` |

---

## 2. Requirements

**Image cleanup stage**

- **PowerShell 5.1+** (default on Windows 11).
- **Docker CLI** on `PATH` (typically after Docker Desktop install).
- **Administrator rights are not required** (a user that can control Docker is enough).

**VHDX compaction stage (`-CompactDockerDataVhdx`)**

- **PowerShell elevated (Run as administrator)**.
- **Hyper-V** PowerShell module (`Optimize-VHD`), same as for manual VHD maintenance.
- **Docker Desktop** with default WSL2 data layout (see below).

---

## 3. Two stages

### 3.1. Image cleanup (default)

1. **`docker image prune -f`** — removes dangling layers (`<none>`), unless `-SkipDanglingPrune` is set.
2. **`docker images`** — for each tagged image, the **REPOSITORY** field is checked:
   - matches a “keep” rule → **skip**;
   - otherwise → **`docker rmi repository:tag`**.
3. **`docker system df`** — short usage summary inside the Docker daemon.

**Note:** if a container still uses an image, `docker rmi` fails for that reference — expected. Stop/remove containers and retry if needed.

### 3.2. Compacting `docker_data.vhdx` (`-CompactDockerDataVhdx`)

Deleting layers frees space **inside** the virtual disk, but the **`.vhdx` file size on Windows does not shrink by itself**. Compaction:

1. Tries to exit **Docker Desktop** gracefully (`Docker Desktop.exe --quit`, wait up to `DockerQuitWaitSeconds` seconds).
2. Runs **`wsl --shutdown`**, then waits `WslShutdownWaitSeconds` seconds.
3. Runs **`Optimize-VHD -Path …\docker_data.vhdx -Mode Full`**.

Afterwards, **start Docker Desktop manually**.

Default file path:

`%LOCALAPPDATA%\Docker\wsl\disk\docker_data.vhdx`

Override with **`-DockerDataVhdxPath`** if needed.

---

## 4. Default “keep” rules

An image is **kept** if **REPOSITORY** (as shown by `docker images`) matches **any** of the built-in regexes:

| Pattern (meaning) |
|-------------------|
| `ubuntu`, `debian`, `fedora`, `docker` (including **dind** tags), `alpine`, `busybox` — short form **or** `docker.io/library/…` |
| `registry.gitlab.com/gitlab-org/release-cli` |

**Everything else** (including your private registry and CI images) is **removed** by default.

Add **`-ExtraKeepPattern`** (string or array of regexes) for more exceptions.

---

## 5. Parameters (short)

| Parameter | Purpose |
|-----------|---------|
| `-KeepRepositoryRegex` | Replace the built-in “base image” regex list |
| `-ExtraKeepPattern` | Additional “do not remove” regexes |
| `-SkipDanglingPrune` | Skip `docker image prune -f` |
| `-SkipImagePrune` | Skip the whole image stage; only meaningful with `-CompactDockerDataVhdx` |
| `-CompactDockerDataVhdx` | Compact `docker_data.vhdx` after (or instead of) image cleanup |
| `-DockerDataVhdxPath` | Full path to the `.vhdx` if non-default |
| `-DockerQuitWaitSeconds` | Wait for Docker to exit (default 90) |
| `-WslShutdownWaitSeconds` | Sleep after `wsl --shutdown` before `Optimize-VHD` (default 15) |
| `-WhatIf` | Preview destructive actions where supported |
| `-Confirm` | Prompt per `ShouldProcess` operation |

---

## 6. Typical commands

Dry run:

```powershell
cd <repo_root>
.\scripts\ci\docker-prune-keep-bases.ps1 -WhatIf
```

Images only (no VHDX shrink on Windows):

```powershell
.\scripts\ci\docker-prune-keep-bases.ps1
```

Images + VHDX compaction (**elevated PowerShell**):

```powershell
.\scripts\ci\docker-prune-keep-bases.ps1 -CompactDockerDataVhdx
```

Also keep Microsoft Container Registry images:

```powershell
.\scripts\ci\docker-prune-keep-bases.ps1 -ExtraKeepPattern '^mcr\.microsoft\.com/'
```

VHDX only (images already cleaned):

```powershell
.\scripts\ci\docker-prune-keep-bases.ps1 -SkipImagePrune -CompactDockerDataVhdx
```

Slower Docker/WSL shutdown:

```powershell
.\scripts\ci\docker-prune-keep-bases.ps1 -CompactDockerDataVhdx -DockerQuitWaitSeconds 120 -WslShutdownWaitSeconds 25
```

---

## 7. Built-in PowerShell help

```powershell
Get-Help .\scripts\ci\docker-prune-keep-bases.ps1 -Full
Get-Help .\scripts\ci\docker-prune-keep-bases.ps1 -Examples
Get-Help .\scripts\ci\docker-prune-keep-bases.ps1 -Parameter CompactDockerDataVhdx
```

The text matches the comment-based help at the top of the `.ps1` file.

---

## 8. FAQ

**Deletion or `docker system df` takes a long time**

Possible on huge stores / slow disks. Avoid heavy parallel Docker work. If it looks stuck, check for mass `docker rmi` and cancel with `Ctrl+C` if needed.

**Images disappeared but `C:` did not grow**

Run **`-CompactDockerDataVhdx`** (admin + Hyper-V). Confirm you compact the real `docker_data.vhdx`.

**`Optimize-VHD` fails**

- Run as **administrator**.
- Install the **Hyper-V** module.
- Ensure Docker/WSL are fully stopped (on “file in use”, quit Docker manually, run `wsl --shutdown`, retry).

**Keep custom base images**

Add **`-ExtraKeepPattern`** matching the full **REPOSITORY** string from `docker images` (include registry host/port).

---

## 9. Automation (optional)

- **Task Scheduler** → run `powershell.exe` with `-File` and the full path to `docker-prune-keep-bases.ps1`.
- For **`-CompactDockerDataVhdx`**, enable **Run with highest privileges**.

---

## 10. Safety

- The script **deletes local images** that do not match the rules. Use **`-WhatIf`** first.
- VHDX compaction runs **`wsl --shutdown`**, which affects **all** WSL distros, not only Docker.
- Schedule a maintenance window on shared runners.

---

## 11. Related documentation

- GitLab CI pipeline and architecture: [docs/en/CI_PIPELINE.md](../../docs/en/CI_PIPELINE.md)
- Full `scripts/ci` reference: [docs/en/CI_SCRIPTS.md](../../docs/en/CI_SCRIPTS.md)
