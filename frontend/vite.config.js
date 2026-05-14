import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";
import { resolveDevProxyTarget } from "./src/utils/devProxyTarget.js";

export default defineConfig(async () => {
  const apiProxyTarget = await resolveDevProxyTarget();

  return {
    plugins: [vue()],
    server: {
      host: "0.0.0.0",
      port: 5173,
      proxy: {
        "/api": {
          target: apiProxyTarget,
          changeOrigin: true,
        },
      },
    },
    test: {
      environment: "jsdom",
      globals: true,
      setupFiles: "./src/test/setup.js",
    },
  };
});
