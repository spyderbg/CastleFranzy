import test from "node:test";
import assert from "node:assert/strict";
import { EventEmitter } from "node:events";
import type { ServerResponse } from "node:http";
import { setTimeout as delay } from "node:timers/promises";
import { PageLifetime } from "../backend/page-lifetime";
import { stripWindowsSignature } from "../scripts/packaging";

function page() {
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

test("last-tab shutdown tolerates refresh, other tabs and an active build", async () => {
  let stopped = 0;
  let busy = false;
  const lifetime = new PageLifetime(
    () => stopped++,
    () => busy,
    30,
  );
  try {
    await delay(45);
    assert.equal(stopped, 0, "--no-open waits indefinitely for its first tab");
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
    assert.equal(stopped, 0, "refresh reconnected within the grace period");
    busy = true;
    refreshed.end();
    await delay(70);
    assert.equal(stopped, 0, "the resource job must finish first");
    busy = false;
    await delay(70);
    assert.equal(stopped, 1);
  } finally {
    lifetime.dispose();
  }
});

test("Windows packaging removes the existing certificate without altering PE headers", () => {
  const pe = Buffer.alloc(512);
  pe.write("MZ");
  pe.writeUInt32LE(64, 0x3c);
  pe.write("PE\0\0", 64);
  pe.writeUInt16LE(240, 84);
  pe.writeUInt16LE(0x20b, 88);
  const security = 88 + 112 + 4 * 8;
  pe.writeUInt32LE(480, security);
  pe.writeUInt32LE(32, security + 4);
  const stripped = stripWindowsSignature(pe);
  assert.equal(stripped.length, 480);
  assert.equal(stripped.readUInt32LE(security), 0);
  assert.equal(stripped.readUInt32LE(security + 4), 0);
  assert.equal(pe.readUInt32LE(security), 480, "input is immutable");
  assert.throws(() => stripWindowsSignature(Buffer.from("invalid")), /PE/);
});
