import { test } from "node:test";
import assert from "node:assert/strict";
import path from "node:path";
import { Game } from "../backend/game";
import { Workspace } from "../backend/workspace";
test(
  "real game bridge plays the selected day, pauses, resumes, toggles overlay and damages the gate",
  { skip: process.env.LEVEL_EDITOR_GAME_TEST !== "1", timeout: 60000 },
  async (t) => {
    const game = new Game(new Workspace(path.resolve("../..")));
    t.after(() => game.stop());
    await game.start("Build/bin");
    let status = await game.command("play", 0, 1);
    assert.equal(status.level, 0);
    assert.equal(status.day, 1);
    const deadline = Date.now() + 30000;
    while (!status.valid && Date.now() < deadline) {
      await new Promise((r) => setTimeout(r, 100));
      status = await game.status();
    }
    assert(status.valid, "game loaded the day");
    assert(status.units?.length);
    status = await game.command("pause");
    assert(status.paused);
    const time = status.time;
    await new Promise((r) => setTimeout(r, 350));
    assert.equal((await game.status()).time, time);
    status = await game.command("resume");
    assert.equal(status.paused, false);
    status = await game.command("overlay");
    assert.equal(status.error, "");
    status = await game.command("die");
    assert.equal(status.error, "");
    assert.equal(status.gateHealth, 0);
    // A second Play refreshes cached level/day data without restarting the process.
    status = await game.command("play", 1, 0);
    assert.equal(status.level, 1);
    assert.equal(status.day, 0);
  },
);
