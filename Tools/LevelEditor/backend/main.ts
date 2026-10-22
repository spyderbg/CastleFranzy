import path from "node:path";
import defaults from "../config/defaults.json";
import { existsSync } from "node:fs";
import { spawn } from "node:child_process";
import { Workspace } from "./workspace";
import { createServer } from "./server";

function openBrowser(url: string) {
  const child =
    process.platform === "win32"
      ? spawn(
          "powershell.exe",
          ["-NoProfile", "-Command", "Start-Process $env:LEVEL_EDITOR_URL"],
          { env: { ...process.env, LEVEL_EDITOR_URL: url }, stdio: "ignore" },
        )
      : spawn(process.platform === "darwin" ? "open" : "xdg-open", [url], {
          stdio: "ignore",
        });
  child.on("error", () => console.log(`Open ${url} in your browser.`));
  child.unref();
}
async function main() {
  const STOP_ON_PAGE_CLOSE = process.env.STOP_ON_PAGE_CLOSE ?? "false";
  if (STOP_ON_PAGE_CLOSE !== "true" && STOP_ON_PAGE_CLOSE !== "false")
    throw new Error("STOP_ON_PAGE_CLOSE must be 'true' or 'false'");
  const args = process.argv.slice(2);
  if (args.includes("--help") || args.includes("-h")) {
    console.log(
      "CastleFrenzyLevelEditor — browser level editor\nUsage: level-editor [--workspace directory] [--port number] [--assets project-relative-directory] [--game project-relative-executable] [--no-open]\nDefaults: find the Castle Frenzy project above the current folder, port 5190.",
    );
    return;
  }
  let workspaceArg: string | undefined,
    portArg: string | undefined,
    assetsArg: string | undefined,
    executableArg: string | undefined,
    noOpen = false;
  for (let i = 0; i < args.length; i++) {
    if (args[i] === "--no-open") noOpen = true;
    else if (
      args[i] === "--workspace" ||
      args[i] === "--port" ||
      args[i] === "--assets" ||
      args[i] === "--game"
    ) {
      const option = args[i],
        value = args[++i];
      if (!value || value.startsWith("--"))
        throw new Error(`${option} requires a value`);
      if (option === "--workspace") workspaceArg = value;
      else if (option === "--port") portArg = value;
      else if (option === "--assets") assetsArg = value;
      else executableArg = value;
    } else
      throw new Error(`Unknown argument ${args[i]}. Use --help for usage.`);
  }
  let root = path.resolve(
    workspaceArg || process.env.LEVEL_EDITOR_WORKSPACE || process.cwd(),
  );
  if (!workspaceArg && !process.env.LEVEL_EDITOR_WORKSPACE)
    while (
      !existsSync(path.join(root, "Build/bin/Levels")) &&
      path.dirname(root) !== root
    )
      root = path.dirname(root);
  if (!existsSync(root) || root === path.parse(root).root)
    throw new Error(
      "Choose a project folder with --workspace /path/to/project",
    );
  const dev = process.env.LEVEL_EDITOR_DEV === "1",
    port = Number(portArg || process.env.PORT || defaults.port);
  if (!Number.isInteger(port) || port < 1 || port > 65535)
    throw new Error("Port must be 1..65535");
  const url = dev ? `http://127.0.0.1:${port + 1}` : `http://127.0.0.1:${port}`;
  const shouldOpen = !noOpen && process.env.LEVEL_EDITOR_NO_OPEN !== "1";
  let stopping = false;
  const app = createServer(new Workspace(root, assetsArg || defaults.assets), {
    dev,
    frontendPort: port + 1,
    executable: executableArg,
    ...(STOP_ON_PAGE_CLOSE === "true"
      ? { onShutdown: () => void close() }
      : {}),
    onExit: () => void close(),
  });
  const close = async () => {
    if (stopping) return;
    stopping = true;
    await app.close();
  };
  try {
    await app.listen({ host: "127.0.0.1", port });
  } catch (error) {
    if ((error as NodeJS.ErrnoException).code !== "EADDRINUSE") throw error;
    const existing = await fetch(`http://127.0.0.1:${port}/api/session`, {
      signal: AbortSignal.timeout(1500),
    })
      .then((r) => r.json())
      .catch(() => null);
    if (
      existing?.app !== "CastleFrenzyLevelEditor" ||
      existing.root !== root ||
      existing.assets !== (assetsArg || defaults.assets) ||
      existing.development !== dev ||
      existing.autoShutdownOnClose !== (STOP_ON_PAGE_CLOSE === "true")
    )
      throw new Error(
        `Port ${port} is already in use. Choose another with --port.`,
      );
    await app.close();
    console.log(`CastleFrenzyLevelEditor is already running at ${url}`);
    if (shouldOpen) openBrowser(url);
    return;
  }
  console.log(`CastleFrenzyLevelEditor is ready at ${url}\nWorkspace: ${root}`);
  if (shouldOpen) openBrowser(url);
  process.on("SIGINT", () => void close());
  process.on("SIGTERM", () => void close());
}
main().catch((error) => {
  console.error(`CastleFrenzyLevelEditor: ${error.message}`);
  process.exitCode = 1;
});
