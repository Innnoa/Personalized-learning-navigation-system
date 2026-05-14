#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
FRONTEND_DIR="${ROOT_DIR}/frontend"
API_PROXY_TARGET="${FRONTEND_API_PROXY_TARGET:-}"

if [[ ! -d "${FRONTEND_DIR}/node_modules" ]]; then
  echo "[frontend] 未检测到依赖，请先运行 ./scripts/install_frontend_deps.sh" >&2
  exit 1
fi

echo "[frontend] dev server: http://localhost:5173"
if [[ -n "${API_PROXY_TARGET}" ]]; then
  echo "[frontend] backend api proxy: ${API_PROXY_TARGET}"
else
  echo "[frontend] backend api proxy: auto-detect (8080 -> 18080)"
fi

cd "${FRONTEND_DIR}"
if [[ -n "${API_PROXY_TARGET}" ]]; then
  VITE_DEV_PROXY_TARGET="${API_PROXY_TARGET}" exec npm run dev
else
  exec npm run dev
fi
