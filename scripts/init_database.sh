#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DB_DIR="${ROOT_DIR}/database/data"
DB_PATH="${DB_DIR}/app.db"
SCHEMA_SQL="${ROOT_DIR}/database/init/001_schema.sql"
SEED_SQL="${ROOT_DIR}/database/seed/course_data_structures.sql"
QUESTION_BANK_SEED_SQL="${ROOT_DIR}/database/seed/question_banks_data_structures.sql"
RESOURCE_SEED_SQL="${ROOT_DIR}/database/seed/learning_resources_data_structures.sql"

build_sanitized_question_bank_seed() {
  local source_sql="$1"
  local target_sql="$2"

  awk '
    /^PRAGMA foreign_keys = ON;$/ { print; next }
    /^INSERT INTO question_banks / && /;$/ { print; next }
    /^INSERT INTO questions / && /;$/ { print; next }
  ' "${source_sql}" > "${target_sql}"
}

ensure_demo_feedback_seed() {
  local existing_count
  existing_count="$(sqlite3 "${DB_PATH}" "
    select count(1)
    from learning_feedback_records r
    join learners l on l.id = r.learner_id
    where l.code = 'demo-learner';
  ")"

  if [[ "${existing_count}" != "0" ]]; then
    return
  fi

  echo "[database] 回填 demo-learner 历史反馈 seed..."
  sqlite3 "${DB_PATH}" "
    INSERT INTO learning_feedback_records (
      learner_id,
      knowledge_point_id,
      feedback_batch_id,
      completion_status,
      self_rated_mastery,
      previous_mastery,
      updated_mastery,
      rule_applied,
      recorded_at
    )
    VALUES
      (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'sequence-list' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'seed-demo-learner-seq',
        'partial',
        0.34,
        0.22,
        0.28,
        'seeded-demo-progress',
        '2026-06-10 09:30:00'
      ),
      (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'linked-list' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'seed-demo-learner-linked',
        'partial',
        0.54,
        0.31,
        0.48,
        'seeded-demo-progress',
        '2026-06-12 15:20:00'
      ),
      (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'queue' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'seed-demo-learner-queue',
        'completed',
        0.61,
        0.39,
        0.56,
        'seeded-demo-progress',
        '2026-06-16 11:40:00'
      ),
      (
        (SELECT id FROM learners WHERE code = 'demo-learner'),
        (SELECT id FROM knowledge_points WHERE code = 'tree-basic' AND course_id = (SELECT id FROM courses WHERE code = 'data-structures')),
        'seed-demo-learner-tree',
        'completed',
        0.88,
        0.43,
        0.92,
        'seeded-demo-progress',
        '2026-06-19 18:10:00'
      );
  "
}

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

if [[ -f "${QUESTION_BANK_SEED_SQL}" ]]; then
  echo "[database] 写入题库 seed 数据（自动过滤损坏记录）..."
  SANITIZED_QUESTION_BANK_SEED_SQL="$(mktemp)"
  trap 'rm -f "${SANITIZED_QUESTION_BANK_SEED_SQL}"' EXIT
  build_sanitized_question_bank_seed "${QUESTION_BANK_SEED_SQL}" "${SANITIZED_QUESTION_BANK_SEED_SQL}"
  sqlite3 "${DB_PATH}" < "${SANITIZED_QUESTION_BANK_SEED_SQL}"
elif [[ -f "${QUESTION_BANK_SEED_SQL}" ]]; then
  :
fi

if [[ -f "${RESOURCE_SEED_SQL}" ]]; then
  echo "[database] 写入学习资源 seed 数据..."
  sqlite3 "${DB_PATH}" < "${RESOURCE_SEED_SQL}"
fi

ensure_demo_feedback_seed

echo "[database] 已生成 ${DB_PATH}"
