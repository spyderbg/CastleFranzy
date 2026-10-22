import { defineConfig } from "@playwright/test";
export default defineConfig({
  testDir: "tests/e2e",
  workers: 1,
  timeout: 60000,
  use: {
    baseURL: "http://127.0.0.1:5192",
    viewport: { width: 1440, height: 1050 },
    screenshot: "only-on-failure",
    trace: "retain-on-failure",
    launchOptions: {
      executablePath: process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE_PATH,
      args: ["--enable-unsafe-swiftshader"],
    },
  },
  webServer: {
    command: "tsx tests/serve.ts",
    url: "http://127.0.0.1:5192/api/session",
    timeout: 30000,
  },
});
