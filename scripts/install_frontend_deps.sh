#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
FRONTEND_DIR="${ROOT_DIR}/frontend"
REGISTRY_URL="${NPM_REGISTRY_URL:-https://registry.npmjs.org/}"
FETCH_TIMEOUT="${NPM_FETCH_TIMEOUT:-20000}"
FETCH_RETRIES="${NPM_FETCH_RETRIES:-2}"
DEFAULT_NO_PROXY="localhost,127.0.0.1,::1"

is_wsl() {
  grep -qi "microsoft" /proc/version 2>/dev/null || [[ -n "${WSL_DISTRO_NAME:-}" ]]
}

read_current_proxy() {
  printf '%s' "${HTTPS_PROXY:-${HTTP_PROXY:-${ALL_PROXY:-${https_proxy:-${http_proxy:-${all_proxy:-}}}}}}"
}

proxy_is_loopback() {
  local proxy_url="${1:-}"
  [[ -n "${proxy_url}" ]] || return 1
  [[ "${proxy_url}" =~ ^https?://(127\.0\.0\.1|localhost)(:[0-9]+)?/?$ ]]
}

get_wsl_host_ip() {
  grep -m1 '^nameserver' /etc/resolv.conf 2>/dev/null | awk '{print $2}'
}

rewrite_proxy_for_wsl_host() {
  local proxy_url="${1:-}"
  local host_ip="${2:-}"

  if [[ -z "${proxy_url}" || -z "${host_ip}" ]]; then
    return 1
  fi

  printf '%s' "${proxy_url}" | sed -E "s#://(127\\.0\\.0\\.1|localhost)#://${host_ip}#"
}

probe_registry_direct() {
  env -u HTTP_PROXY -u HTTPS_PROXY -u ALL_PROXY \
      -u http_proxy -u https_proxy -u all_proxy \
      curl -fsSI --max-time 8 "${REGISTRY_URL}" >/dev/null
}

probe_registry_current() {
  curl -fsSI --max-time 8 "${REGISTRY_URL}" >/dev/null
}

probe_registry_with_proxy() {
  local proxy_url="${1:?proxy url is required}"

  env \
    HTTP_PROXY="${proxy_url}" \
    HTTPS_PROXY="${proxy_url}" \
    ALL_PROXY="${proxy_url}" \
    http_proxy="${proxy_url}" \
    https_proxy="${proxy_url}" \
    all_proxy="${proxy_url}" \
    curl -fsSI --max-time 8 "${REGISTRY_URL}" >/dev/null
}

apply_direct_env() {
  unset HTTP_PROXY HTTPS_PROXY ALL_PROXY
  unset http_proxy https_proxy all_proxy
  export NO_PROXY="${NO_PROXY:-${DEFAULT_NO_PROXY}}"
}

apply_proxy_env() {
  local proxy_url="${1:?proxy url is required}"

  export HTTP_PROXY="${proxy_url}"
  export HTTPS_PROXY="${proxy_url}"
  export ALL_PROXY="${proxy_url}"
  export http_proxy="${proxy_url}"
  export https_proxy="${proxy_url}"
  export all_proxy="${proxy_url}"
  export NO_PROXY="${NO_PROXY:-${DEFAULT_NO_PROXY}}"
}

CURRENT_PROXY="$(read_current_proxy)"
SELECTED_MODE=""
SELECTED_PROXY=""
WSL_HOST_IP=""
WSL_PROXY=""

if [[ -n "${CURRENT_PROXY}" ]] && probe_registry_current; then
  SELECTED_MODE="current-proxy"
  SELECTED_PROXY="${CURRENT_PROXY}"
elif is_wsl && proxy_is_loopback "${CURRENT_PROXY}"; then
  WSL_HOST_IP="$(get_wsl_host_ip)"
  WSL_PROXY="$(rewrite_proxy_for_wsl_host "${CURRENT_PROXY}" "${WSL_HOST_IP}" || true)"

  if [[ -n "${WSL_PROXY}" && "${WSL_PROXY}" != "${CURRENT_PROXY}" ]] && probe_registry_with_proxy "${WSL_PROXY}"; then
    SELECTED_MODE="wsl-host-proxy"
    SELECTED_PROXY="${WSL_PROXY}"
  fi
fi

if [[ -z "${SELECTED_MODE}" ]] && probe_registry_direct; then
  SELECTED_MODE="direct"
fi

if [[ -z "${SELECTED_MODE}" ]]; then
  echo "[frontend] 无法连接 npm registry: ${REGISTRY_URL}" >&2
  echo "[frontend] 已尝试：当前代理、WSL 宿主机代理改写、直连" >&2
  echo "[frontend] 当前代理：${CURRENT_PROXY:-<empty>}" >&2
  if is_wsl; then
    echo "[frontend] WSL nameserver：${WSL_HOST_IP:-<unknown>}" >&2
    echo "[frontend] 请检查 Windows 侧代理是否允许 WSL 访问，或手动执行 'unset HTTP_PROXY HTTPS_PROXY ALL_PROXY' 后重试" >&2
  fi
  exit 1
fi

if [[ "${SELECTED_MODE}" == "direct" ]]; then
  apply_direct_env
else
  apply_proxy_env "${SELECTED_PROXY}"
fi

echo "[frontend] Node: $(node --version)"
echo "[frontend] npm: $(npm --version)"
echo "[frontend] registry: ${REGISTRY_URL}"
echo "[frontend] network mode: ${SELECTED_MODE}"
if [[ -n "${SELECTED_PROXY}" ]]; then
  echo "[frontend] proxy: ${SELECTED_PROXY}"
fi

cd "${FRONTEND_DIR}"

echo "[frontend] npm ping..."
npm ping \
  --registry="${REGISTRY_URL}" \
  --fetch-retries=0 \
  --fetch-timeout=8000 >/dev/null

echo "[frontend] npm install..."
npm install \
  --registry="${REGISTRY_URL}" \
  --no-progress \
  --loglevel=notice \
  --fetch-retries="${FETCH_RETRIES}" \
  --fetch-timeout="${FETCH_TIMEOUT}" \
  --audit=false \
  --fund=false
