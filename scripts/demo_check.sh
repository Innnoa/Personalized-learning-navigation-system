#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

BACKEND_PORT="${DEMO_CHECK_BACKEND_PORT:-${DEMO_BACKEND_PORT:-18080}}"
FRONTEND_PORT="${DEMO_CHECK_FRONTEND_PORT:-${DEMO_FRONTEND_PORT:-5174}}"
API_BASE_URL="${DEMO_CHECK_API_BASE_URL:-http://127.0.0.1:${BACKEND_PORT}}"
FRONTEND_BASE_URL="${DEMO_CHECK_FRONTEND_URL:-http://127.0.0.1:${FRONTEND_PORT}}"
TIMEOUT_SEC="${DEMO_CHECK_TIMEOUT_SEC:-35}"

AUTO_UP="${DEMO_CHECK_AUTO_UP:-1}"
RUN_SMOKE="${DEMO_CHECK_RUN_SMOKE:-1}"
AUTO_DOWN="${DEMO_CHECK_AUTO_DOWN:-0}"
CHECK_REUSE_EXISTING="${DEMO_CHECK_REUSE_EXISTING:-0}"

CHECK_TARGET_CODE="${DEMO_CHECK_TARGET_CODE:-topological-sort}"
CHECK_AVAILABLE_MINUTES="${DEMO_CHECK_AVAILABLE_MINUTES:-90}"
CHECK_LEARNER_CODE="${DEMO_CHECK_LEARNER_CODE:-demo-learner}"

log() {
  echo "[demo-check] $*"
}

fail() {
  echo "[demo-check] ERROR: $*" >&2
  exit 1
}

detect_lan_ipv4() {
  local candidate

  if command -v hostname >/dev/null 2>&1; then
    for candidate in $(hostname -I 2>/dev/null || true); do
      if [[ "${candidate}" =~ ^([0-9]{1,3}\.){3}[0-9]{1,3}$ ]] && [[ "${candidate}" != 127.* ]]; then
        echo "${candidate}"
        return 0
      fi
    done
  fi

  if command -v ip >/dev/null 2>&1; then
    while IFS= read -r candidate; do
      if [[ -n "${candidate}" ]] && [[ "${candidate}" != 127.* ]]; then
        echo "${candidate}"
        return 0
      fi
    done < <(ip -4 -o addr show scope global 2>/dev/null | awk '{split($4, a, "/"); print a[1]}')
  fi

  return 1
}

require_command() {
  local cmd="$1"
  if ! command -v "${cmd}" >/dev/null 2>&1; then
    fail "缺少依赖命令：${cmd}"
  fi
}

wait_http_ready() {
  local url="$1"
  local timeout="$2"
  local deadline=$((SECONDS + timeout))
  local http_code

  while ((SECONDS < deadline)); do
    http_code="$(curl -sS --max-time 1 -o /dev/null -w '%{http_code}' "${url}" 2>/dev/null || true)"
    if [[ "${http_code}" == "200" ]]; then
      return 0
    fi
    sleep 1
  done

  return 1
}

cleanup() {
  if [[ "${AUTO_DOWN}" == "1" ]]; then
    log "AUTO_DOWN=1，执行 demo_down 回收进程..."
    "${ROOT_DIR}/scripts/demo_down.sh" || true
  fi
}
trap cleanup EXIT

run_demo_up_if_needed() {
  if [[ "${AUTO_UP}" != "1" ]]; then
    log "跳过 demo_up（DEMO_CHECK_AUTO_UP=${AUTO_UP}）"
    return 0
  fi

  log "步骤 1/3：启动演示环境（demo_up）"
  DEMO_BACKEND_PORT="${BACKEND_PORT}" \
  DEMO_FRONTEND_PORT="${FRONTEND_PORT}" \
  DEMO_REUSE_EXISTING="${CHECK_REUSE_EXISTING}" \
  DEMO_START_TIMEOUT_SEC="${TIMEOUT_SEC}" \
  "${ROOT_DIR}/scripts/demo_up.sh"
}

check_services() {
  log "步骤 2/3：检查前后端可访问性"
  if ! wait_http_ready "${API_BASE_URL}/api/health" "${TIMEOUT_SEC}"; then
    fail "后端健康检查未通过：${API_BASE_URL}/api/health"
  fi
  if ! wait_http_ready "${FRONTEND_BASE_URL}" "${TIMEOUT_SEC}"; then
    fail "前端页面不可访问：${FRONTEND_BASE_URL}"
  fi
  log "前后端服务状态正常。"
}

run_smoke_if_needed() {
  if [[ "${RUN_SMOKE}" != "1" ]]; then
    log "跳过后端冒烟（DEMO_CHECK_RUN_SMOKE=${RUN_SMOKE}）"
    return 0
  fi

  log "步骤 3/3：执行后端主链路冒烟（smoke_demo_flow）"
  SMOKE_REUSE_EXISTING_BACKEND=1 \
  API_BASE_URL="${API_BASE_URL}" \
  TARGET_CODE="${CHECK_TARGET_CODE}" \
  AVAILABLE_MINUTES="${CHECK_AVAILABLE_MINUTES}" \
  LEARNER_CODE="${CHECK_LEARNER_CODE}" \
  START_TIMEOUT_SEC="${TIMEOUT_SEC}" \
  "${ROOT_DIR}/scripts/smoke_demo_flow.sh"
}

print_summary() {
  local lan_ip=""
  local localhost_frontend_url="http://127.0.0.1:${FRONTEND_PORT}"
  local localhost_backend_url="http://127.0.0.1:${BACKEND_PORT}"

  lan_ip="$(detect_lan_ipv4 || true)"

  log "✅ demo_check 通过"
  echo "  前端本机访问: ${localhost_frontend_url}"
  if [[ -n "${lan_ip}" ]]; then
    echo "  前端局域网访问: http://${lan_ip}:${FRONTEND_PORT}"
  else
    echo "  前端局域网访问: 未检测到局域网 IPv4"
  fi
  echo "  后端本机访问: ${localhost_backend_url}"
  if [[ -n "${lan_ip}" ]]; then
    echo "  后端局域网访问: http://${lan_ip}:${BACKEND_PORT}"
  else
    echo "  后端局域网访问: 未检测到局域网 IPv4"
  fi
  echo "  当前检查前端地址: ${FRONTEND_BASE_URL}"
  echo "  当前检查后端地址: ${API_BASE_URL}"
  echo "  复用现有实例: ${CHECK_REUSE_EXISTING}"
  echo "  目标节点: ${CHECK_TARGET_CODE}"
  echo "  时间预算: ${CHECK_AVAILABLE_MINUTES}"
  if [[ "${AUTO_DOWN}" == "1" ]]; then
    echo "  进程回收: 已启用（结束时自动执行 demo_down）"
  else
    echo "  进程回收: 未启用（可手动执行 ./scripts/demo_check_down.sh 回收 ${BACKEND_PORT}/${FRONTEND_PORT}）"
  fi
}

main() {
  require_command curl
  require_command bash

  run_demo_up_if_needed
  check_services
  run_smoke_if_needed
  print_summary
}

main "$@"
