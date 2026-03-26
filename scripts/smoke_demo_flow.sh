#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BACKEND_DIR="${ROOT_DIR}/backend"
BUILD_DIR="${BACKEND_DIR}/build"
BACKEND_BIN="${BUILD_DIR}/backend"
BACKEND_CONFIG="${BACKEND_DIR}/config/config.json"

REUSE_EXISTING_BACKEND="${SMOKE_REUSE_EXISTING_BACKEND:-0}"
BACKEND_PORT="${SMOKE_BACKEND_PORT:-18080}"
API_BASE_URL="${API_BASE_URL:-}"
if [[ -z "${API_BASE_URL}" ]]; then
  if [[ "${REUSE_EXISTING_BACKEND}" == "1" ]]; then
    API_BASE_URL="http://127.0.0.1:8080"
  else
    API_BASE_URL="http://127.0.0.1:${BACKEND_PORT}"
  fi
fi
LEARNER_CODE="${LEARNER_CODE:-demo-learner}"
TARGET_CODE="${TARGET_CODE:-topological-sort}"
AVAILABLE_MINUTES="${AVAILABLE_MINUTES:-90}"
START_TIMEOUT_SEC="${START_TIMEOUT_SEC:-25}"

TMP_DIR="$(mktemp -d)"
BACKEND_PID=""
STARTED_BACKEND=0
RUNTIME_CONFIG_PATH=""

cleanup() {
  if [[ "${STARTED_BACKEND}" -eq 1 && -n "${BACKEND_PID}" ]]; then
    kill "${BACKEND_PID}" >/dev/null 2>&1 || true
    wait "${BACKEND_PID}" 2>/dev/null || true
  fi
  rm -rf "${TMP_DIR}"
}
trap cleanup EXIT

log() {
  echo "[smoke] $*"
}

fail() {
  local message="$1"
  local response_file="${2:-}"
  echo "[smoke] ERROR: ${message}" >&2
  if [[ -n "${response_file}" && -f "${response_file}" ]]; then
    echo "[smoke] 响应内容：" >&2
    cat "${response_file}" >&2
  fi
  if [[ -f "${TMP_DIR}/backend.log" ]]; then
    echo "[smoke] backend 日志（最近 40 行）：" >&2
    tail -n 40 "${TMP_DIR}/backend.log" >&2 || true
  fi
  exit 1
}

require_command() {
  local name="$1"
  if ! command -v "${name}" >/dev/null 2>&1; then
    fail "缺少依赖命令：${name}"
  fi
}

json_status_ok() {
  local file="$1"
  grep -Eq '"status"[[:space:]]*:[[:space:]]*"ok"' "${file}"
}

api_call() {
  local method="$1"
  local endpoint="$2"
  local body="${3:-}"
  local output_file="$4"
  local url="${API_BASE_URL}${endpoint}"
  local http_code

  if [[ "${method}" == "GET" ]]; then
    http_code="$(curl -sS -o "${output_file}" -w '%{http_code}' "${url}" || true)"
  else
    http_code="$(
      curl -sS \
        -o "${output_file}" \
        -w '%{http_code}' \
        -X "${method}" \
        -H 'Content-Type: application/json' \
        -d "${body}" \
        "${url}" || true
    )"
  fi

  if [[ "${http_code}" != "200" ]]; then
    fail "${method} ${endpoint} 返回 HTTP ${http_code}" "${output_file}"
  fi

  if ! json_status_ok "${output_file}"; then
    fail "${method} ${endpoint} 返回 status != ok" "${output_file}"
  fi
}

health_up() {
  local output_file="${TMP_DIR}/health_probe.json"
  local http_code
  http_code="$(
    curl -sS \
      -o "${output_file}" \
      -w '%{http_code}' \
      "${API_BASE_URL}/api/health" || true
  )"

  [[ "${http_code}" == "200" ]] && json_status_ok "${output_file}"
}

wait_backend_ready() {
  local deadline=$((SECONDS + START_TIMEOUT_SEC))
  while ((SECONDS < deadline)); do
    if health_up; then
      return 0
    fi
    sleep 1
  done
  return 1
}

start_backend_if_needed() {
  if [[ "${REUSE_EXISTING_BACKEND}" == "1" ]]; then
    if ! health_up; then
      fail "已启用复用模式，但未检测到可用后端：${API_BASE_URL}"
    fi
    log "复用已有后端：${API_BASE_URL}"
    return 0
  fi

  log "初始化数据库并启动独立后端实例（端口 ${BACKEND_PORT}）..."
  "${ROOT_DIR}/scripts/init_database.sh"

  cmake -S "${BACKEND_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j"$(nproc)"

  RUNTIME_CONFIG_PATH="${TMP_DIR}/smoke_backend_config.json"
  cp "${BACKEND_CONFIG}" "${RUNTIME_CONFIG_PATH}"
  sed -i "s/\"port\": 8080/\"port\": ${BACKEND_PORT}/" "${RUNTIME_CONFIG_PATH}"
  sed -i \
    "s#\"filename\": \"../../database/data/app.db\"#\"filename\": \"${ROOT_DIR}/database/data/app.db\"#" \
    "${RUNTIME_CONFIG_PATH}"

  (
    cd "${BUILD_DIR}"
    "${BACKEND_BIN}" "${RUNTIME_CONFIG_PATH}"
  ) >"${TMP_DIR}/backend.log" 2>&1 &
  BACKEND_PID=$!
  STARTED_BACKEND=1

  if ! wait_backend_ready; then
    fail "后端在 ${START_TIMEOUT_SEC}s 内未就绪。"
  fi
  log "本地后端已就绪（pid=${BACKEND_PID}）"
}

assert_non_empty_path() {
  local file="$1"
  if ! grep -Eq '"path"[[:space:]]*:[[:space:]]*\[' "${file}"; then
    fail "路径规划响应中缺少 path 数组" "${file}"
  fi
  if grep -Eq '"path"[[:space:]]*:[[:space:]]*\[[[:space:]]*\]' "${file}"; then
    fail "路径规划响应 path 为空" "${file}"
  fi
}

assert_has_resource_recommendations() {
  local file="$1"
  if ! grep -Eq '"resourceRecommendations"[[:space:]]*:[[:space:]]*\[' "${file}"; then
    fail "路径规划响应中缺少 resourceRecommendations" "${file}"
  fi
}

assert_positive_counter_field() {
  local file="$1"
  local field="$2"
  if ! grep -Eq "\"${field}\"[[:space:]]*:[[:space:]]*[1-9][0-9]*" "${file}"; then
    fail "响应中的 ${field} 不是正整数" "${file}"
  fi
}

main() {
  require_command curl
  require_command cmake
  require_command nproc

  start_backend_if_needed

  log "1/6 健康检查"
  api_call "GET" "/api/health" "" "${TMP_DIR}/step1_health.json"

  log "2/6 恢复演示初始状态"
  api_call \
    "POST" \
    "/api/demo/reset" \
    "{\"learnerCode\":\"${LEARNER_CODE}\"}" \
    "${TMP_DIR}/step2_reset.json"

  log "3/6 生成学习路径"
  api_call \
    "POST" \
    "/api/path/generate" \
    "{\"learnerCode\":\"${LEARNER_CODE}\",\"targetCodes\":[\"${TARGET_CODE}\"],\"availableMinutes\":${AVAILABLE_MINUTES}}" \
    "${TMP_DIR}/step3_generate.json"
  assert_non_empty_path "${TMP_DIR}/step3_generate.json"
  assert_has_resource_recommendations "${TMP_DIR}/step3_generate.json"

  log "4/6 提交一条学习反馈"
  api_call \
    "POST" \
    "/api/feedback/submit" \
    "{\"learnerCode\":\"${LEARNER_CODE}\",\"feedbackItems\":[{\"code\":\"graph-basic\",\"completionStatus\":\"partial\",\"selfRatedMastery\":0.55}]}" \
    "${TMP_DIR}/step4_feedback_submit.json"
  assert_positive_counter_field "${TMP_DIR}/step4_feedback_submit.json" "savedRecordCount"

  log "5/6 回退最近一次反馈"
  api_call \
    "POST" \
    "/api/feedback/rollback" \
    "{\"learnerCode\":\"${LEARNER_CODE}\"}" \
    "${TMP_DIR}/step5_feedback_rollback.json"
  assert_positive_counter_field "${TMP_DIR}/step5_feedback_rollback.json" "rolledBackRecordCount"

  log "6/6 读取学习者画像"
  api_call \
    "GET" \
    "/api/learner/profile?learnerCode=${LEARNER_CODE}" \
    "" \
    "${TMP_DIR}/step6_profile.json"
  if ! grep -Eq '"graphMasteryByCode"[[:space:]]*:' "${TMP_DIR}/step6_profile.json"; then
    fail "学习者画像响应缺少 graphMasteryByCode" "${TMP_DIR}/step6_profile.json"
  fi
  if ! grep -Eq '"detailMasteryByCode"[[:space:]]*:[[:space:]]*\{' "${TMP_DIR}/step6_profile.json"; then
    fail "学习者画像响应缺少 detailMasteryByCode 对象" "${TMP_DIR}/step6_profile.json"
  fi

  log "✅ 冒烟通过：health/reset/generate/feedback/rollback/profile 全链路正常。"
}

main "$@"
