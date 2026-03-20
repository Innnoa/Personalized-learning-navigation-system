#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/backend/build"

cmake -S "${ROOT_DIR}/backend" -B "${BUILD_DIR}"
cmake --build "${BUILD_DIR}" -j"$(nproc)"

cd "${BUILD_DIR}"
./backend ../config/config.json
