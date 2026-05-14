const DEFAULT_PROXY_TARGET = "http://127.0.0.1:8080";
const DEV_PROXY_CANDIDATES = [DEFAULT_PROXY_TARGET, "http://127.0.0.1:18080"];

async function probeBackendTarget(target) {
  const controller = new AbortController();
  const timeoutId = setTimeout(() => controller.abort(), 300);

  try {
    const response = await fetch(`${target}/api/health`, {
      signal: controller.signal,
    });
    return response.ok;
  } catch {
    return false;
  } finally {
    clearTimeout(timeoutId);
  }
}

export async function resolveDevProxyTarget({
  env = process.env,
  probe = probeBackendTarget,
} = {}) {
  if (env.VITE_DEV_PROXY_TARGET) {
    return env.VITE_DEV_PROXY_TARGET;
  }

  for (const target of DEV_PROXY_CANDIDATES) {
    if (await probe(target)) {
      return target;
    }
  }

  return DEFAULT_PROXY_TARGET;
}

export { DEFAULT_PROXY_TARGET, DEV_PROXY_CANDIDATES, probeBackendTarget };
