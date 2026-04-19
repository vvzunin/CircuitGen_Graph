# Reference: `scripts/ci`

This document is **kept in sync** across the Parameters, Graph, and Generator repositories. Script behaviour is the same; GitLab jobs inject variables (`REPO_NAME`, `DOCKER_URL`, …) from `.gitlab-ci.yml`.

Pipeline overview: [CI_PIPELINE.md](CI_PIPELINE.md).

---

## 1. Running tasks locally

| Script | Purpose |
|--------|---------|
| **`run-task.sh`** | Single task: `bash scripts/ci/run-task.sh lint`, etc. Environment: `CI_RUNNER=local\|docker`, `CI_IMAGE_TAG`, `TARGET_OS`. |
| **`run-all.sh`** | Runs the main CI scripts in sequence (similar to a typical pipeline). |

See `run-task.sh` for the supported task names (`lint`, `sanitize`, `static-analysis`, `coverage`, `tests`, `examples`, `docs`, …).

---

## 2. Docker image build and push

| Script | Purpose |
|--------|---------|
| **`docker-build-ci.sh`** | Build and push the **CI** image (`buildx`), driven by `docker-paths.sh` / CI variables. |
| **`docker-build-dev.sh`** | Build the **dev** image (based on the CI image). |
| **`docker-build-release.sh`** | Build the **release** image. |
| **`docker-registry-login.sh`** | `docker login` to the CI registry (GitLab CI/CD variables). |
| **`docker-skip-if-unchanged.sh`** | `docker manifest inspect` plus `docker-context-changed.sh`; exit 0 means skip the build. Argument: `ci` \| `dev` \| `release`. |
| **`docker-context-changed.sh`** | `git diff` over paths that affect an image layer; argument: `ci` \| `dev` \| `release`. |
| **`docker-hub-proxy-image.sh`** | Rewrites a short Docker Hub reference (`ubuntu:24.04`) to a Harbor proxy path (`REGISTRY_URL`/`DOCKER_HUB_PROXY_PROJECT`). |

---

## 3. `.gitlab-ci.yml` generation and validation

| Script | Purpose |
|--------|---------|
| **`generate-gitlab-os-matrix.sh`** | Generates OS matrix jobs between markers in `.gitlab-ci.yml`. Modes: no args (stdout), `--write`, `--check`. |

---

## 4. Code checks (main entrypoints)

| Script | Purpose |
|--------|---------|
| **`lint.sh`** | Linting (format, project rules). |
| **`static-analysis.sh`** | Static analysis (e.g. cppcheck). |
| **`sanitize.sh`** | Build/tests with sanitizers. |
| **`tests.sh`** | Run tests (CTest, etc.). |
| **`coverage.sh`** | Code coverage. |
| **`examples.sh`** | Build/run examples. |
| **`docs.sh`** | Documentation build in CI. |

---

## 5. OS images and full checks

| Script | Purpose |
|--------|---------|
| **`os-image-build-push.sh`** | Build a “clean” OS image and push to the registry (os-check stage). |
| **`os-full-check.sh`** | Full validation on an OS image (install-deps, checks). |

---

## 6. Other

| Script | Purpose |
|--------|---------|
| **`create-gitlab-release.sh`** | Create a GitLab Release (**release** stage, tags). |
| **`apk-install-with-retry.sh`** | Install **apk** packages on Alpine with retries (helper for `before_script`). |

---

## 7. Windows: runner disk maintenance

| File | Purpose |
|------|---------|
| **`docker-prune-keep-bases.ps1`** | PowerShell: remove local images except base images; optionally **compact** `docker_data.vhdx` (`Optimize-VHD`). |
| **`docker-prune-keep-bases.md`** | Manual (Russian). |
| **`docker-prune-keep-bases.en.md`** | Manual (English). |

Built-in help: `Get-Help .\scripts\ci\docker-prune-keep-bases.ps1 -Full`.

---

## 8. Related directories

| Directory | Role |
|-----------|------|
| `scripts/config` | `supported-os.sh` — OS matrix for CI. |
| `scripts/docker` | `docker-paths.sh` and registry/path helpers. |
| `scripts/setup` | OS dependency installers (used by Dockerfiles and os-check). |
| `scripts/dev`, `scripts/release`, `scripts/docs` | Not GitLab CI entrypoints from `.gitlab-ci.yml`, but related to dev/release/docs; see [SCRIPTS.md](SCRIPTS.md). |
