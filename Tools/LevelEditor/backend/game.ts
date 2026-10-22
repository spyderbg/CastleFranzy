import path from "node:path";
import { spawn, type ChildProcess } from "node:child_process";
import { mkdir, readFile, rm, stat, open } from "node:fs/promises";
import { randomBytes } from "node:crypto";
import type { GameStatus } from "../shared/level";
import { Workspace, atomicWrite } from "./workspace";
export class Game {
  private child?: ChildProcess;
  private directory = "";
  private assets = "";
  private seq = 0;
  private log = "";
  private queue = Promise.resolve();
  constructor(
    private workspace: Workspace,
    private executable?: string,
  ) {}
  async start(assets: string) {
    if (this.child) {
      if (this.assets !== assets)
        throw new Error("Stop the test game before switching assets");
      return;
    }
    const candidates = this.executable
      ? [this.executable]
      : process.platform === "win32"
        ? [
            "build/windows-x64/bin/Debug/FortressDefender.exe",
            "build/windows-x64/bin/Release/FortressDefender.exe",
          ]
        : [
            "build/linux-debug/bin/FortressDefender",
            "build/linux-release/bin/FortressDefender",
          ];
    let binary = "";
    for (const candidate of candidates) {
      try {
        binary = await this.workspace.contained(candidate);
        break;
      } catch {}
    }
    if (!binary)
      throw new Error(
        "Build the game first: cmake --build --preset linux-debug",
      );
    // Detect old executables before launching a window that cannot accept commands.
    if (!(await readFile(binary)).includes(Buffer.from("--editor-bridge")))
      throw new Error("Rebuild the game to enable the Level Editor bridge");
    const assetPath = await this.workspace.assets(assets);
    this.directory = path.join(
      this.workspace.root,
      ".level-editor",
      "sessions",
      randomBytes(12).toString("hex"),
    );
    await mkdir(this.directory, { recursive: true, mode: 0o700 });
    this.assets = assets;
    this.seq = 0;
    this.log = "";
    const logFile = await open(path.join(this.directory, "game.log"), "a");
    const child = spawn(
      binary,
      [
        "--assets",
        assetPath,
        "--editor-bridge",
        this.directory,
        "--save-dir",
        path.join(this.directory, "save"),
      ],
      {
        cwd: this.workspace.root,
        detached: process.platform !== "win32",
        stdio: ["ignore", logFile.fd, logFile.fd],
      },
    );
    await logFile.close();
    this.child = child;
    child.on("error", (error) => {
      this.log += error.message;
      if (this.child === child) this.child = undefined;
    });
    child.on("exit", (code) => {
      this.log += `\nGame exited (${code})`;
      if (this.child === child) this.child = undefined;
    });
  }
  async status(): Promise<GameStatus> {
    let status: Partial<GameStatus> = {};
    if (this.directory)
      try {
        const file = path.join(this.directory, "status.json");
        if (Date.now() - (await stat(file)).mtimeMs < 5000)
          status = JSON.parse(await readFile(file, "utf8"));
      } catch {}
    if (this.directory)
      try {
        const bytes = await readFile(path.join(this.directory, "game.log"));
        this.log = bytes.subarray(-12000).toString();
      } catch {}
    return { ...status, running: !!this.child, log: this.log };
  }
  command(command: string, level = 0, day = 0): Promise<GameStatus> {
    const run = this.queue.then(async () => {
      if (!this.child) throw new Error("No test game is running");
      const seq = ++this.seq;
      await atomicWrite(
        path.join(this.directory, "command.txt"),
        `${seq} ${command} ${level} ${day}\n`,
      );
      const deadline = Date.now() + 30000;
      while (Date.now() < deadline) {
        const status = await this.status();
        if (!status.running)
          throw new Error(`Game stopped. ${status.log?.slice(-1000)}`);
        if ((status.seq ?? 0) >= seq) {
          if (status.error) throw new Error(status.error);
          return status;
        }
        await new Promise((resolve) => setTimeout(resolve, 100));
      }
      throw new Error(
        "Game did not respond within 30 seconds. Check the game log.",
      );
    });
    this.queue = run.then(
      () => {},
      () => {},
    );
    return run;
  }
  async stop() {
    const child = this.child;
    if (child) {
      child.kill();
      await new Promise<void>((resolve) => {
        const timer = setTimeout(() => {
          child.kill("SIGKILL");
          resolve();
        }, 3000);
        child.once("exit", () => {
          clearTimeout(timer);
          resolve();
        });
      });
      if (this.child === child) this.child = undefined;
    }
    if (this.directory)
      await rm(this.directory, { recursive: true, force: true });
    this.directory = "";
  }
  async detach() {
    if (this.child) {
      try {
        await this.command("resume");
      } catch {}
      this.child.unref();
      this.child = undefined;
    }
    // The game keeps its bridge/save/log files until it exits.
    this.directory = "";
  }
}
