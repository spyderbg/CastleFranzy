import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";
import defaults from "./config/defaults.json";
const backendPort = Number(process.env.PORT || defaults.port);
export default defineConfig({
  root: "frontend",
  plugins: [vue()],
  server: {
    host: "127.0.0.1",
    port: backendPort + 1,
    strictPort: true,
    proxy: { "/api": `http://127.0.0.1:${backendPort}` },
  },
  build: { outDir: "../build/frontend", emptyOutDir: true },
});
