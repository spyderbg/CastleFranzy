import { build as viteBuild } from "vite";
import { bundleBackend } from "./backend-build";
import { mkdir, readFile, readdir, writeFile } from "node:fs/promises";
import path from "node:path";
await viteBuild();
const files: Record<string, { mime: string; base64: string }> = {};
const types: Record<string, string> = {
  ".html": "text/html",
  ".js": "application/javascript",
  ".css": "text/css",
  ".svg": "image/svg+xml",
  ".png": "image/png",
};
async function walk(dir: string) {
  for (const item of await readdir(path.join("build/frontend", dir), {
    withFileTypes: true,
  })) {
    const file = path.join(dir, item.name);
    if (item.isDirectory()) await walk(file);
    else
      files["/" + file.split(path.sep).join("/")] = {
        mime: types[path.extname(file)] || "application/octet-stream",
        base64: (await readFile(path.join("build/frontend", file))).toString(
          "base64",
        ),
      };
  }
}
await walk("");
await mkdir("build", { recursive: true });
await writeFile(
  "build/embedded.ts",
  `export const embeddedFiles = ${JSON.stringify(files)};`,
);
await bundleBackend("build/server.cjs");
console.log("Built Level Editor frontend and backend.");
