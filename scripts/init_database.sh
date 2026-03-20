#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DB_DIR="${ROOT_DIR}/database/data"
DB_PATH="${DB_DIR}/app.db"
SCHEMA_SQL="${ROOT_DIR}/database/init/001_schema.sql"
SEED_SQL="${ROOT_DIR}/database/seed/course_data_structures.sql"

if ! command -v sqlite3 >/dev/null 2>&1; then
  echo "[database] 未检测到 sqlite3，请先安装 sqlite。" >&2
  exit 1
fi

mkdir -p "${DB_DIR}"
rm -f "${DB_PATH}"

echo "[database] 初始化 schema..."
sqlite3 "${DB_PATH}" < "${SCHEMA_SQL}"

echo "[database] 写入 seed 数据..."
sqlite3 "${DB_PATH}" < "${SEED_SQL}"

echo "[database] 已生成 ${DB_PATH}"
