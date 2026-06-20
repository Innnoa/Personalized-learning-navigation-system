#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SCRIPT_DIR="${ROOT_DIR}/scripts"
APT_UPDATED=0

log() {
  echo "[ubuntu-install] $*"
}

fail() {
  echo "[ubuntu-install] ERROR: $*" >&2
  exit 1
}

command_exists() {
  command -v "$1" >/dev/null 2>&1
}

require_ubuntu() {
  if [[ ! -f /etc/os-release ]]; then
    fail "未检测到 /etc/os-release，无法确认系统发行版。"
  fi

  # shellcheck disable=SC1091
  source /etc/os-release

  if [[ "${ID:-}" != "ubuntu" ]]; then
    fail "该脚本仅面向 Ubuntu，当前系统为：${PRETTY_NAME:-unknown}"
  fi
}

setup_privilege_runner() {
  if [[ "${EUID}" -eq 0 ]]; then
    SUDO_CMD=()
    return
  fi

  if ! command_exists sudo; then
    fail "需要 root 或 sudo 权限执行 apt 安装。"
  fi

  SUDO_CMD=(sudo)
}

apt_update_once() {
  if [[ "${APT_UPDATED}" -eq 1 ]]; then
    return
  fi

  log "更新 apt 索引..."
  "${SUDO_CMD[@]}" apt-get update
  APT_UPDATED=1
}

apt_install() {
  local packages=("$@")

  apt_update_once
  log "安装系统包: ${packages[*]}"
  "${SUDO_CMD[@]}" apt-get install -y "${packages[@]}"
}

apt_install_first_available() {
  local pkg=""

  if ! command_exists apt-cache; then
    fail "缺少 apt-cache，无法自动探测可用的 apt 包。"
  fi

  for pkg in "$@"; do
    if apt-cache show "${pkg}" >/dev/null 2>&1; then
      apt_install "${pkg}"
      return 0
    fi
  done

  return 1
}

detect_node_major() {
  if ! command_exists node; then
    return 1
  fi

  node -p "process.versions.node.split('.')[0]" 2>/dev/null
}

ensure_base_packages() {
  apt_install \
    ca-certificates \
    curl \
    git \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    ccache \
    sqlite3 \
    libsqlite3-dev \
    libssl-dev \
    uuid-dev \
    libpq-dev \
    zlib1g-dev \
    libjsoncpp-dev

  if ! apt_install_first_available \
    default-libmysqlclient-dev \
    libmysqlclient-dev \
    libmariadb-dev; then
    log "未在当前 apt 源中找到 MySQL/MariaDB 开发包，后续若 Drogon 依赖检查报 MySQL 缺失，请手动安装对应开发包。"
  fi
}

ensure_nodejs() {
  local node_major=""

  if node_major="$(detect_node_major || true)" && [[ -n "${node_major}" ]] && (( node_major >= 18 )); then
    log "检测到 Node.js v${node_major}，满足前端依赖要求。"
    return
  fi

  log "安装 Node.js 20.x（用于前端 Vite / Vitest）。"
  apt_install gnupg

  local nodesource_script
  nodesource_script="$(mktemp)"
  trap 'rm -f "${nodesource_script}"' EXIT

  curl -fsSL https://deb.nodesource.com/setup_20.x -o "${nodesource_script}"
  "${SUDO_CMD[@]}" bash "${nodesource_script}"
  apt_install nodejs

  local installed_major
  installed_major="$(detect_node_major || true)"
  if [[ -z "${installed_major}" ]] || (( installed_major < 18 )); then
    fail "Node.js 安装后版本仍低于 18，请检查 NodeSource 源是否生效。"
  fi
}

print_tool_versions() {
  log "g++: $(g++ --version | head -n 1)"
  log "cmake: $(cmake --version | head -n 1)"
  log "sqlite3: $(sqlite3 --version | awk '{print $1}')"
  log "node: $(node --version)"
  log "npm: $(npm --version)"
}

install_frontend_deps() {
  log "安装前端 npm 依赖..."
  "${SCRIPT_DIR}/install_frontend_deps.sh"
}

detect_drogon_package() {
  local candidate

  if ! command_exists apt-cache; then
    return 1
  fi

  for candidate in libdrogon-dev drogon-dev; do
    if apt-cache show "${candidate}" >/dev/null 2>&1; then
      printf '%s\n' "${candidate}"
      return 0
    fi
  done

  return 1
}

install_drogon_package_if_available() {
  local drogon_pkg=""

  drogon_pkg="$(detect_drogon_package || true)"
  if [[ -z "${drogon_pkg}" ]]; then
    return 1
  fi

  log "安装 Drogon 开发包: ${drogon_pkg}"
  apt_install "${drogon_pkg}"
  return 0
}

print_drogon_help() {
  local pkg_name="${1:-}"

  echo
  echo "[ubuntu-install] 后端 Drogon 依赖尚未就绪。"
  if [[ -n "${pkg_name}" ]]; then
    echo "[ubuntu-install] 已尝试自动安装 ${pkg_name}，但 CMake 仍未检测通过。"
  else
    echo "[ubuntu-install] 当前 apt 源中未探测到明确的 Drogon 开发包。"
  fi
  echo "[ubuntu-install] 若你已自行安装 Drogon，请通过 CMAKE_PREFIX_PATH 或 Drogon_DIR 指向 DrogonConfig.cmake。"
  echo "[ubuntu-install] 也可先手动确认：apt-cache search drogon"
}

check_backend_deps() {
  log "检查后端 Drogon 构建依赖..."

  # shellcheck disable=SC1091
  source "${SCRIPT_DIR}/lib_backend_deps.sh"

  if require_backend_build_dependencies "${ROOT_DIR}"; then
    log "后端 Drogon 依赖检查通过。"
    return
  fi

  if install_drogon_package_if_available; then
    if require_backend_build_dependencies "${ROOT_DIR}"; then
      log "后端 Drogon 依赖检查通过。"
      return
    fi

    print_drogon_help "$(detect_drogon_package || true)"
    return 1
  fi

  print_drogon_help
  return 1
}

print_next_steps() {
  echo
  echo "[ubuntu-install] 下一步："
  echo "  1. ./scripts/init_database.sh"
  echo "  2. ./scripts/run_backend.sh"
  echo "  3. ./scripts/run_frontend.sh"
  echo
  echo "[ubuntu-install] 或直接启动演示环境："
  echo "  ./scripts/demo_up.sh"
}

main() {
  require_ubuntu
  setup_privilege_runner
  ensure_base_packages
  ensure_nodejs
  print_tool_versions
  install_frontend_deps
  check_backend_deps
  print_next_steps
}

main "$@"
