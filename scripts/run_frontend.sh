#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
FRONTEND_DIR="${ROOT_DIR}/frontend"
API_PROXY_TARGET="${FRONTEND_API_PROXY_TARGET:-http://127.0.0.1:8080}"

if [[ ! -d "${FRONTEND_DIR}/node_modules" ]]; then
  echo "[frontend] 未检测到依赖，请先运行 ./scripts/install_frontend_deps.sh" >&2
  exit 1
fi

echo "[frontend] dev server: http://localhost:5173"
echo "[frontend] backend api proxy: ${API_PROXY_TARGET}"

cd "${FRONTEND_DIR}"
VITE_DEV_PROXY_TARGET="${API_PROXY_TARGET}" exec npm run dev
