#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/backend/build"

source "${ROOT_DIR}/scripts/lib_backend_deps.sh"

prepare_backend_build_env "${BUILD_DIR}"

require_backend_build_dependencies "${ROOT_DIR}" || {
  echo "[backend] 后端构建依赖检查未通过。" >&2
  exit 1
}

cmake --fresh -S "${ROOT_DIR}/backend" -B "${BUILD_DIR}"
cmake --build "${BUILD_DIR}" -j"$(nproc)"

cd "${BUILD_DIR}"
./backend ../config/config.json
