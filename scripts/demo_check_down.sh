#!/usr/bin/env bash
set -euo pipefail

BACKEND_PORT="${DEMO_CHECK_BACKEND_PORT:-${DEMO_BACKEND_PORT:-18080}}"
FRONTEND_PORT="${DEMO_CHECK_FRONTEND_PORT:-${DEMO_FRONTEND_PORT:-5174}}"
STOP_TIMEOUT_SEC="${DEMO_CHECK_DOWN_TIMEOUT_SEC:-6}"

log() {
  echo "[demo-check-down] $*"
}

fail() {
  echo "[demo-check-down] ERROR: $*" >&2
  exit 1
}

require_command() {
  local cmd="$1"
  if ! command -v "${cmd}" >/dev/null 2>&1; then
    fail "缺少依赖命令：${cmd}"
  fi
}

port_pids() {
  local port="$1"
  fuser "${port}/tcp" 2>/dev/null | xargs echo -n
}

is_port_busy() {
  local port="$1"
  [[ -n "$(port_pids "${port}")" ]]
}

wait_port_released() {
  local port="$1"
  local timeout_sec="$2"
  local deadline=$((SECONDS + timeout_sec))

  while ((SECONDS < deadline)); do
    if ! is_port_busy "${port}"; then
      return 0
    fi
    sleep 0.2
  done

  return 1
}

stop_port() {
  local port="$1"
  local service_name="$2"
  local pids

  pids="$(port_pids "${port}")"
  if [[ -z "${pids}" ]]; then
    log "${service_name} 未占用端口 ${port}，跳过。"
    return 0
  fi

  log "尝试停止 ${service_name}（端口 ${port}，pid=${pids}）..."
  kill ${pids} >/dev/null 2>&1 || true

  if wait_port_released "${port}" "${STOP_TIMEOUT_SEC}"; then
    log "${service_name} 已停止（端口 ${port} 已释放）。"
    return 0
  fi

  pids="$(port_pids "${port}")"
  if [[ -n "${pids}" ]]; then
    log "${service_name} 未在 ${STOP_TIMEOUT_SEC}s 内退出，执行强制结束（pid=${pids}）..."
    kill -9 ${pids} >/dev/null 2>&1 || true
  fi

  if wait_port_released "${port}" 2; then
    log "${service_name} 已强制停止（端口 ${port} 已释放）。"
    return 0
  fi

  fail "${service_name} 端口 ${port} 仍被占用，请手动排查。"
}

main() {
  require_command fuser
  require_command xargs

  stop_port "${FRONTEND_PORT}" "demo_check 前端"
  stop_port "${BACKEND_PORT}" "demo_check 后端"

  log "✅ demo_check 对应端口已完成回收"
  echo "  前端端口: ${FRONTEND_PORT}"
  echo "  后端端口: ${BACKEND_PORT}"
}

main "$@"
