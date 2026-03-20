#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
FRONTEND_DIR="${ROOT_DIR}/frontend"

if [[ ! -d "${FRONTEND_DIR}/node_modules" ]]; then
  echo "[frontend] 未检测到依赖，请先运行 ./scripts/install_frontend_deps.sh" >&2
  exit 1
fi

echo "[frontend] dev server: http://localhost:5173"
echo "[frontend] backend api: http://127.0.0.1:8080"

cd "${FRONTEND_DIR}"
exec npm run dev
