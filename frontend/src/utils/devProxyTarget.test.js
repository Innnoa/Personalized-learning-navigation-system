import { describe, expect, it, vi } from "vitest";

import { resolveDevProxyTarget } from "./devProxyTarget";

describe("resolveDevProxyTarget", () => {
  it("prefers an explicit env override", async () => {
    const probe = vi.fn();

    const target = await resolveDevProxyTarget({
      env: { VITE_DEV_PROXY_TARGET: "http://127.0.0.1:19080" },
      probe,
    });

    expect(target).toBe("http://127.0.0.1:19080");
    expect(probe).not.toHaveBeenCalled();
  });

  it("auto-detects the demo backend on 18080 before falling back to 8080", async () => {
    const probe = vi.fn(async (target) => target === "http://127.0.0.1:18080");

    const target = await resolveDevProxyTarget({ env: {}, probe });

    expect(target).toBe("http://127.0.0.1:18080");
    expect(probe.mock.calls.map(([value]) => value)).toEqual([
      "http://127.0.0.1:8080",
      "http://127.0.0.1:18080",
    ]);
  });

  it("falls back to 8080 when no candidate responds", async () => {
    const probe = vi.fn(async () => false);

    const target = await resolveDevProxyTarget({ env: {}, probe });

    expect(target).toBe("http://127.0.0.1:8080");
  });
});
