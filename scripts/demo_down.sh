#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RUNTIME_DIR="${ROOT_DIR}/backend/build/.demo-runtime"
BACKEND_PID_FILE="${RUNTIME_DIR}/backend.pid"
FRONTEND_PID_FILE="${RUNTIME_DIR}/frontend.pid"

log() {
  echo "[demo-down] $*"
}

stop_pid_file() {
  local pid_file="$1"
  local service_name="$2"

  if [[ ! -f "${pid_file}" ]]; then
    log "${service_name} 无 pid 文件，跳过。"
    return 0
  fi

  local pid
  pid="$(cat "${pid_file}")"
  if [[ -z "${pid}" ]]; then
    rm -f "${pid_file}"
    log "${service_name} pid 文件为空，已清理。"
    return 0
  fi

  if ! kill -0 "${pid}" >/dev/null 2>&1; then
    rm -f "${pid_file}"
    log "${service_name} 进程不存在（pid=${pid}），已清理 pid 文件。"
    return 0
  fi

  kill "${pid}" >/dev/null 2>&1 || true
  for _ in {1..20}; do
    if ! kill -0 "${pid}" >/dev/null 2>&1; then
      rm -f "${pid_file}"
      log "${service_name} 已停止（pid=${pid}）。"
      return 0
    fi
    sleep 0.2
  done

  kill -9 "${pid}" >/dev/null 2>&1 || true
  rm -f "${pid_file}"
  log "${service_name} 未在超时内退出，已强制结束（pid=${pid}）。"
}

main() {
  if [[ ! -d "${RUNTIME_DIR}" ]]; then
    log "未检测到运行时目录，可能无需停止。"
    exit 0
  fi

  stop_pid_file "${FRONTEND_PID_FILE}" "前端"
  stop_pid_file "${BACKEND_PID_FILE}" "后端"

  # 仅清理空目录，避免误删日志
  rmdir "${RUNTIME_DIR}" 2>/dev/null || true

  log "✅ 停止流程完成。"
}

main "$@"
