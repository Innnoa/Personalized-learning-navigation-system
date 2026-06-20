#!/usr/bin/env bash

get_project_root() {
  local script_dir
  script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
  cd "${script_dir}/.." && pwd
}

detect_drogon_config_dir() {
  local candidate

  for candidate in \
    "${Drogon_DIR:-}" \
    "/usr/lib/cmake/Drogon" \
    "/usr/lib/x86_64-linux-gnu/cmake/Drogon" \
    "/usr/local/lib/cmake/Drogon" \
    "/usr/local/lib64/cmake/Drogon"
  do
    if [[ -n "${candidate}" && -f "${candidate}/DrogonConfig.cmake" ]]; then
      printf '%s\n' "${candidate}"
      return 0
    fi
  done

  return 1
}

prepare_backend_build_env() {
  local build_dir="$1"
  local ccache_dir="${build_dir}/.ccache"
  local ccache_tmp_dir="${ccache_dir}/tmp"
  local default_cmake_library_path="/usr/lib:/usr/lib/x86_64-linux-gnu:/usr/local/lib:/usr/local/lib64"
  local default_cmake_include_path="/usr/include:/usr/include/mysql:/usr/include/postgresql:/usr/local/include"
  local default_cmake_prefix_path="/usr/lib/cmake:/usr/lib/x86_64-linux-gnu/cmake:/usr/local/lib/cmake:/usr/local/lib64/cmake"

  mkdir -p "${ccache_tmp_dir}"

  export CCACHE_DIR="${CCACHE_DIR:-${ccache_dir}}"
  export CCACHE_TEMPDIR="${CCACHE_TEMPDIR:-${ccache_tmp_dir}}"
  export CMAKE_LIBRARY_PATH="${CMAKE_LIBRARY_PATH:-${default_cmake_library_path}}"
  export CMAKE_INCLUDE_PATH="${CMAKE_INCLUDE_PATH:-${default_cmake_include_path}}"
  export CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH:-${default_cmake_prefix_path}}"
}

require_drogon_package() {
  local root_dir="${1:-$(get_project_root)}"
  local probe_build_dir="${root_dir}/backend/build/.drogon-probe"
  local probe_log_file="${probe_build_dir}.log"
  local drogon_dir=""

  prepare_backend_build_env "${root_dir}/backend/build"
  drogon_dir="$(detect_drogon_config_dir || true)"

  if env CCACHE_DISABLE=1 cmake \
    -S "${root_dir}/scripts/cmake/drogon_probe" \
    -B "${probe_build_dir}" \
    ${drogon_dir:+-DDrogon_DIR="${drogon_dir}"} \
    >"${probe_log_file}" 2>&1; then
    return 0
  fi

  cat >&2 <<'EOF'
[backend-deps] 未检测到可用的 Drogon CMake 依赖链。
[backend-deps] 请先安装 Drogon 开发包，并确认其依赖（如 jsoncpp、zlib、sqlite3、uuid、libpq、mysqlclient/mariadb）可被 CMake 发现。
[backend-deps] 也可以通过 CMAKE_PREFIX_PATH / Drogon_DIR 指向 DrogonConfig.cmake。
[backend-deps] Arch Linux / AUR 可执行：paru -S drogon
[backend-deps] 也可执行：yay -S drogon
EOF
  if [[ -n "${drogon_dir}" ]]; then
    echo "[backend-deps] 已探测到 DrogonConfig.cmake: ${drogon_dir}/DrogonConfig.cmake" >&2
  fi
  if [[ -f "${probe_log_file}" ]]; then
    echo "[backend-deps] CMake probe 日志: ${probe_log_file}" >&2
    tail -n 20 "${probe_log_file}" >&2 || true
  fi
  return 1
}

require_backend_build_dependencies() {
  local root_dir="${1:-$(get_project_root)}"

  require_drogon_package "${root_dir}"
}
