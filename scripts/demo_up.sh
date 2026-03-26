#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BACKEND_DIR="${ROOT_DIR}/backend"
FRONTEND_DIR="${ROOT_DIR}/frontend"
BUILD_DIR="${BACKEND_DIR}/build"
BACKEND_BIN="${BUILD_DIR}/backend"
BACKEND_CONFIG="${BACKEND_DIR}/config/config.json"
RUNTIME_DIR="${BUILD_DIR}/.demo-runtime"

BACKEND_PORT="${DEMO_BACKEND_PORT:-8080}"
FRONTEND_PORT="${DEMO_FRONTEND_PORT:-5173}"
AUTO_RESET="${DEMO_AUTO_RESET:-1}"
START_TIMEOUT_SEC="${DEMO_START_TIMEOUT_SEC:-35}"
REUSE_EXISTING="${DEMO_REUSE_EXISTING:-1}"

API_BASE_URL="http://127.0.0.1:${BACKEND_PORT}"
FRONTEND_BASE_URL="http://127.0.0.1:${FRONTEND_PORT}"

BACKEND_PID_FILE="${RUNTIME_DIR}/backend.pid"
FRONTEND_PID_FILE="${RUNTIME_DIR}/frontend.pid"
BACKEND_LOG_FILE="${RUNTIME_DIR}/backend.log"
FRONTEND_LOG_FILE="${RUNTIME_DIR}/frontend.log"

log() {
  echo "[demo-up] $*"
}

fail() {
  echo "[demo-up] ERROR: $*" >&2
  exit 1
}

require_command() {
  local name="$1"
  if ! command -v "${name}" >/dev/null 2>&1; then
    fail "缺少依赖命令：${name}"
  fi
}

is_pid_running() {
  local pid="$1"
  kill -0 "${pid}" >/dev/null 2>&1
}

is_service_ready() {
  local url="$1"
  local output_file="$2"
  local http_code

  http_code="$(
    curl -sS --max-time 1 -o "${output_file}" -w '%{http_code}' "${url}" 2>/dev/null || true
  )"
  [[ "${http_code}" == "200" ]]
}

wait_http_ready() {
  local url="$1"
  local timeout_sec="$2"
  local output_file="$3"
  local deadline=$((SECONDS + timeout_sec))

  while ((SECONDS < deadline)); do
    if is_service_ready "${url}" "${output_file}"; then
      return 0
    fi
    sleep 1
  done

  return 1
}

start_backend() {
  if wait_http_ready "${API_BASE_URL}/api/health" 1 "${RUNTIME_DIR}/backend.health.json"; then
    if [[ "${REUSE_EXISTING}" == "1" ]]; then
      log "检测到后端已可用，复用现有实例：${API_BASE_URL}"
      return 0
    fi
    fail "端口 ${BACKEND_PORT} 已有后端服务。请改用其他端口，或设置 DEMO_REUSE_EXISTING=1 复用该服务。"
  fi

  log "初始化数据库..."
  "${ROOT_DIR}/scripts/init_database.sh"

  log "构建后端..."
  cmake -S "${BACKEND_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j"$(nproc)"

  log "启动后端（端口 ${BACKEND_PORT}）..."
  if [[ "${BACKEND_PORT}" == "8080" ]]; then
    cd "${BUILD_DIR}"
    nohup "${BACKEND_BIN}" "${BACKEND_CONFIG}" >>"${BACKEND_LOG_FILE}" 2>&1 &
  else
    local runtime_config="${RUNTIME_DIR}/demo_backend_config.json"
    cp "${BACKEND_CONFIG}" "${runtime_config}"
    sed -i "s/\"port\": 8080/\"port\": ${BACKEND_PORT}/" "${runtime_config}"
    sed -i \
      "s#\"filename\": \"../../database/data/app.db\"#\"filename\": \"${ROOT_DIR}/database/data/app.db\"#" \
      "${runtime_config}"
    cd "${BUILD_DIR}"
    nohup "${BACKEND_BIN}" "${runtime_config}" >>"${BACKEND_LOG_FILE}" 2>&1 &
  fi

  local backend_pid=$!
  cd "${ROOT_DIR}"
  echo "${backend_pid}" >"${BACKEND_PID_FILE}"

  if ! wait_http_ready "${API_BASE_URL}/api/health" "${START_TIMEOUT_SEC}" "${RUNTIME_DIR}/backend.health.json"; then
    tail -n 40 "${BACKEND_LOG_FILE}" >&2 || true
    fail "后端在 ${START_TIMEOUT_SEC}s 内未就绪。"
  fi

  log "后端已就绪（pid=${backend_pid}）"

  if [[ "${AUTO_RESET}" == "1" ]]; then
    log "执行演示状态重置..."
    local reset_http
    reset_http="$(
      curl -sS \
        -o "${RUNTIME_DIR}/demo.reset.json" \
        -w '%{http_code}' \
        -X POST \
        -H 'Content-Type: application/json' \
        -d '{"learnerCode":"demo-learner"}' \
        "${API_BASE_URL}/api/demo/reset" || true
    )"
    if [[ "${reset_http}" != "200" ]] || ! grep -Eq '"status"[[:space:]]*:[[:space:]]*"ok"' "${RUNTIME_DIR}/demo.reset.json"; then
      cat "${RUNTIME_DIR}/demo.reset.json" >&2 || true
      fail "自动演示重置失败，请检查数据库权限与后端日志。"
    fi
  fi
}

start_frontend() {
  if wait_http_ready "${FRONTEND_BASE_URL}" 1 "${RUNTIME_DIR}/frontend.health.html"; then
    if [[ "${REUSE_EXISTING}" == "1" ]]; then
      log "检测到前端已可用，复用现有实例：${FRONTEND_BASE_URL}"
      return 0
    fi
    fail "端口 ${FRONTEND_PORT} 已有前端服务。请改用其他端口，或设置 DEMO_REUSE_EXISTING=1 复用该服务。"
  fi

  if [[ ! -d "${FRONTEND_DIR}/node_modules" ]]; then
    log "未检测到前端依赖，自动安装..."
    "${ROOT_DIR}/scripts/install_frontend_deps.sh"
  fi

  log "启动前端（端口 ${FRONTEND_PORT}）..."
  cd "${FRONTEND_DIR}"
  nohup npm run dev -- --host --port "${FRONTEND_PORT}" >>"${FRONTEND_LOG_FILE}" 2>&1 &

  local frontend_pid=$!
  cd "${ROOT_DIR}"
  echo "${frontend_pid}" >"${FRONTEND_PID_FILE}"

  if ! wait_http_ready "${FRONTEND_BASE_URL}" "${START_TIMEOUT_SEC}" "${RUNTIME_DIR}/frontend.health.html"; then
    tail -n 40 "${FRONTEND_LOG_FILE}" >&2 || true
    fail "前端在 ${START_TIMEOUT_SEC}s 内未就绪。"
  fi

  log "前端已就绪（pid=${frontend_pid}）"
}

print_summary() {
  log "✅ 演示环境已启动"
  echo "  前端地址: ${FRONTEND_BASE_URL}"
  echo "  后端地址: ${API_BASE_URL}"
  echo "  后端日志: ${BACKEND_LOG_FILE}"
  echo "  前端日志: ${FRONTEND_LOG_FILE}"
  echo "  停止命令: ./scripts/demo_down.sh"
}

main() {
  require_command curl
  require_command cmake
  require_command nproc
  require_command npm

  mkdir -p "${RUNTIME_DIR}"
  : >"${BACKEND_LOG_FILE}"
  : >"${FRONTEND_LOG_FILE}"

  start_backend
  start_frontend

  # 防止 pid 文件记录到已退出进程
  if [[ -f "${BACKEND_PID_FILE}" ]]; then
    local backend_pid
    backend_pid="$(cat "${BACKEND_PID_FILE}")"
    if ! is_pid_running "${backend_pid}"; then
      fail "后端进程已退出，请查看日志：${BACKEND_LOG_FILE}"
    fi
  fi
  if [[ -f "${FRONTEND_PID_FILE}" ]]; then
    local frontend_pid
    frontend_pid="$(cat "${FRONTEND_PID_FILE}")"
    if ! is_pid_running "${frontend_pid}"; then
      fail "前端进程已退出，请查看日志：${FRONTEND_LOG_FILE}"
    fi
  fi

  print_summary
}

main "$@"
