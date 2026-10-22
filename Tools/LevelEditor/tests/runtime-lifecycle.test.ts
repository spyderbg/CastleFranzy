import test, { type TestContext } from "node:test";
import assert from "node:assert/strict";
import { spawn } from "node:child_process";
import { once } from "node:events";
import { createServer } from "node:net";
import path from "node:path";
import { setTimeout as delay } from "node:timers/promises";

async function launch(
  t: TestContext,
  entry: string[],
  overrides: Record<string, string> = {},
  executable = process.execPath,
) {
  const reservation = createServer();
  reservation.listen(0, "127.0.0.1");
  await once(reservation, "listening");
  const port = (reservation.address() as { port: number }).port;
  await new Promise<void>((resolve) => reservation.close(() => resolve()));
  const env = { ...process.env };
  delete env.STOP_ON_PAGE_CLOSE;
  delete env.LEVEL_EDITOR_DEV;
  Object.assign(env, overrides);
  const child = spawn(
    executable,
    [
      ...entry,
      "--workspace",
      path.resolve("../.."),
      "--port",
      String(port),
      "--no-open",
    ],
    { env, stdio: ["ignore", "pipe", "pipe"] },
  );
  const exited = once(child, "exit");
  let log = "";
  child.stdout.on("data", (data) => {
    log = (log + data).slice(-8000);
  });
  child.stderr.on("data", (data) => {
    log = (log + data).slice(-8000);
  });
  t.after(async () => {
    if (child.exitCode === null && child.signalCode === null) child.kill();
    const timer = setTimeout(() => child.kill("SIGKILL"), 3000);
    try {
      await exited;
    } finally {
      clearTimeout(timer);
    }
  });
  const address = `http://127.0.0.1:${port}`;
  let response: Response | undefined;
  for (let i = 0; i < 50; i++) {
    if (child.exitCode !== null) break;
    try {
      response = await fetch(`${address}/api/session`, {
        signal: AbortSignal.timeout(1000),
      });
      break;
    } catch {
      await delay(100);
    }
  }
  assert(response?.ok, log || "Backend failed to start");
  const session = await response.json();
  return { address, session, response, child, exited, log: () => log };
}

const sourceEntry = ["--import", "tsx", "backend/main.ts"];

test(
  "Node defaults to persistent and explicit Exit still stops the process",
  { timeout: 15000 },
  async (t) => {
    const running = await launch(t, sourceEntry);
    assert.equal(running.session.autoShutdownOnClose, false);
    assert.equal((await fetch(`${running.address}/api/lifecycle`)).status, 503);
    const result = await fetch(`${running.address}/api/exit`, {
      method: "POST",
      headers: { "x-level-editor-token": running.session.token },
    });
    assert.equal(result.status, 200);
    const [code] = await running.exited;
    assert.equal(code, 0, running.log());
  },
);

test(
  "STOP_ON_PAGE_CLOSE=true stops development on its last page close",
  { timeout: 15000 },
  async (t) => {
    const running = await launch(t, sourceEntry, {
      STOP_ON_PAGE_CLOSE: "true",
      LEVEL_EDITOR_DEV: "1",
    });
    assert.equal(running.session.development, true);
    assert.equal(running.session.autoShutdownOnClose, true);
    const stream = await fetch(`${running.address}/api/lifecycle`, {
      headers: {
        cookie: running.response.headers.get("set-cookie")!.split(";")[0],
      },
    });
    const reader = stream.body!.getReader();
    await reader.read();
    await reader.cancel();
    const [code] = await running.exited;
    assert.equal(code, 0, running.log());
  },
);

test(
  "explicit false disables shutdown and invalid parameter values fail startup",
  { timeout: 15000 },
  async (t) => {
    const running = await launch(t, sourceEntry, {
      STOP_ON_PAGE_CLOSE: "false",
    });
    assert.equal(running.session.autoShutdownOnClose, false);
    const invalid = spawn(process.execPath, [...sourceEntry, "--no-open"], {
      env: { ...process.env, STOP_ON_PAGE_CLOSE: "1" },
    });
    let error = "";
    invalid.stderr.on("data", (data) => {
      error += data;
    });
    const [code] = await once(invalid, "exit");
    assert.equal(code, 1);
    assert.match(error, /STOP_ON_PAGE_CLOSE must be 'true' or 'false'/);
  },
);

test(
  "standalone forces true despite runtime false, and packaging leaves Node persistent",
  { skip: process.env.LEVEL_EDITOR_PACKAGE_TEST !== "1", timeout: 20000 },
  async (t) => {
    const running = await launch(
      t,
      [],
      { STOP_ON_PAGE_CLOSE: "false" },
      path.resolve("dist/level-editor"),
    );
    assert.equal(running.session.autoShutdownOnClose, true);
    const stream = await fetch(`${running.address}/api/lifecycle`, {
      headers: {
        cookie: running.response.headers.get("set-cookie")!.split(";")[0],
      },
    });
    const reader = stream.body!.getReader();
    await reader.read();
    await reader.cancel();
    const [code] = await running.exited;
    assert.equal(code, 0, running.log());
    const ordinary = await launch(t, ["build/server.cjs"]);
    assert.equal(ordinary.session.autoShutdownOnClose, false);
  },
);
