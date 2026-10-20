import { spawn } from "node:child_process";
import { createServer } from "vite";
const frontend = await createServer({
  server: { open: process.env.AURORA_MX_NO_OPEN !== "1" },
});
await frontend.listen();
const backend = spawn(
  process.execPath,
  ["--watch", "--import", "tsx", "backend/main.ts"],
  {
    stdio: "inherit",
    env: { ...process.env, AURORA_MX_DEV: "1", AURORA_MX_NO_OPEN: "1" },
  },
);
let closing = false;
async function close() {
  if (closing) return;
  closing = true;
  backend.kill("SIGTERM");
  await frontend.close();
}
process.on("SIGINT", close);
process.on("SIGTERM", close);
backend.on("exit", async (code) => {
  await close();
  process.exitCode = code || 0;
});
