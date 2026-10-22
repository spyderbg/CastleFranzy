import { mkdir, cp, mkdtemp, symlink, rm } from "node:fs/promises";
import path from "node:path";
import { spawn } from "node:child_process";
const root = path.resolve("../..");
await mkdir(path.join(root, "build"), { recursive: true });
const fixture = await mkdtemp(path.join(root, "build/level-editor-e2e-"));
await cp(path.join(root, "Build/bin/Levels"), path.join(fixture, "Levels"), {
  recursive: true,
});
await symlink(
  path.join(root, "Build/bin/config"),
  path.join(fixture, "config"),
  "dir",
);
for (const name of ["icons.tga", "icons.png"])
  try {
    await symlink(path.join(root, "Build/bin", name), path.join(fixture, name));
  } catch {}
const child = spawn(
  process.execPath,
  [
    "build/server.cjs",
    "--workspace",
    root,
    "--assets",
    path.relative(root, fixture),
    "--port",
    "5192",
    "--no-open",
  ],
  { stdio: "inherit" },
);
let closing = false;
const close = async () => {
  if (closing) return;
  closing = true;
  child.kill();
  await rm(fixture, { recursive: true, force: true });
};
child.on("exit", close);
process.on("SIGINT", close);
process.on("SIGTERM", close);
