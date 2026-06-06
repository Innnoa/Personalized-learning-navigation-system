#!/usr/bin/env bash

get_project_root() {
  local script_dir
  script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
  cd "${script_dir}/.." && pwd
}

prepare_backend_build_env() {
  local build_dir="$1"
  local ccache_dir="${build_dir}/.ccache"
  local ccache_tmp_dir="${ccache_dir}/tmp"

  mkdir -p "${ccache_tmp_dir}"

  export CCACHE_DIR="${CCACHE_DIR:-${ccache_dir}}"
  export CCACHE_TEMPDIR="${CCACHE_TEMPDIR:-${ccache_tmp_dir}}"
}

require_drogon_package() {
  local root_dir="${1:-$(get_project_root)}"
  local probe_build_dir="${root_dir}/backend/build/.drogon-probe"

  prepare_backend_build_env "${root_dir}/backend/build"

  if cmake -S "${root_dir}/scripts/cmake/drogon_probe" -B "${probe_build_dir}" >/dev/null 2>&1; then
    return 0
  fi

  cat >&2 <<'EOF'
[backend-deps] 未检测到可用的 Drogon CMake 依赖链。
[backend-deps] 请先安装 Drogon 开发包，并确认其依赖（如 jsoncpp、zlib、sqlite3）可被 CMake 发现。
[backend-deps] 也可以通过 CMAKE_PREFIX_PATH / Drogon_DIR 指向 DrogonConfig.cmake。
[backend-deps] Arch Linux / AUR 可执行：paru -S drogon
[backend-deps] 也可执行：yay -S drogon
EOF
  return 1
}

require_backend_build_dependencies() {
  local root_dir="${1:-$(get_project_root)}"

  require_drogon_package "${root_dir}"
}
