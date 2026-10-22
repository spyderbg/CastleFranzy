import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";
export default defineConfig({
  root: "frontend",
  plugins: [vue()],
  server: {
    host: "127.0.0.1",
    port: Number(process.env.PORT || 5190) + 1,
    strictPort: true,
    proxy: { "/api": `http://127.0.0.1:${process.env.PORT || 5190}` },
  },
  build: { outDir: "../build/frontend", emptyOutDir: true },
});
