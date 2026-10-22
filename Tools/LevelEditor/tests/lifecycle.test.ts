import test from "node:test";
import assert from "node:assert/strict";
import { EventEmitter } from "node:events";
import type { ServerResponse } from "node:http";
import { setTimeout as delay } from "node:timers/promises";
import path from "node:path";
import { PageLifetime } from "../backend/page-lifetime";
import { createServer } from "../backend/server";
import { Workspace } from "../backend/workspace";

function page(): ServerResponse {
  const response = new EventEmitter() as EventEmitter & {
    writeHead: () => void;
    write: () => void;
    end: () => void;
  };
  response.writeHead = response.write = () => {};
  response.end = () => {
    response.emit("close");
  };
  return response as unknown as ServerResponse;
}

test("last-page shutdown waits for the first page, other tabs, refreshes and busy work", async () => {
  let stopped = 0,
    busy = false;
  const lifetime = new PageLifetime(
    () => stopped++,
    () => busy,
    30,
  );
  try {
    await delay(45);
    assert.equal(stopped, 0, "no page has connected yet");
    const first = page(),
      second = page();
    lifetime.connect(first);
    lifetime.connect(second);
    first.end();
    await delay(45);
    assert.equal(stopped, 0, "another tab remains open");
    second.end();
    const refreshed = page();
    lifetime.connect(refreshed);
    await delay(45);
    assert.equal(stopped, 0, "refresh reconnected during the grace period");
    busy = true;
    refreshed.end();
    await delay(70);
    assert.equal(stopped, 0, "a save or game command is still running");
    busy = false;
    await delay(70);
    assert.equal(stopped, 1);
  } finally {
    lifetime.dispose();
  }
});

test("server lifetime depends only on its callback, including development mode", async (t) => {
  let stopped = 0;
  const app = createServer(new Workspace(path.resolve("../..")), {
    dev: true,
    onShutdown: () => stopped++,
  });
  t.after(() => app.close());
  const address = await app.listen({ host: "127.0.0.1", port: 0 });
  const session = await fetch(`${address}/api/session`);
  assert.equal((await session.json()).autoShutdownOnClose, true);
  assert.equal((await fetch(`${address}/api/lifecycle`)).status, 403);
  const stream = await fetch(`${address}/api/lifecycle`, {
    headers: { cookie: session.headers.get("set-cookie")!.split(";")[0] },
  });
  assert.equal(stream.status, 200);
  const reader = stream.body!.getReader();
  assert.match(
    new TextDecoder().decode((await reader.read()).value),
    /connected/,
  );
  await reader.cancel();
  await delay(2300);
  assert.equal(stopped, 1);
});

test("persistent servers disable lifecycle streams but retain explicit Exit", async (t) => {
  let exited = 0;
  const app = createServer(new Workspace(path.resolve("../..")), {
    onExit: () => exited++,
  });
  t.after(() => app.close());
  const session = (await app.inject("/api/session")).json();
  assert.equal(session.autoShutdownOnClose, false);
  assert.equal((await app.inject("/api/lifecycle")).statusCode, 503);
  const response = await app.inject({
    method: "POST",
    url: "/api/exit",
    headers: { "x-level-editor-token": session.token },
    payload: {},
  });
  assert.equal(response.statusCode, 200);
  await new Promise<void>((resolve) => setImmediate(resolve));
  assert.equal(exited, 1);
});
