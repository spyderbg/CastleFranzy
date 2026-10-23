import { build } from "esbuild";
import path from "node:path";

/** Keep ordinary Node bundles configurable; SEA builds fix the desktop lifetime. */
export async function bundleBackend(outfile: string, stopOnPageClose?: "true") {
  await build({
    entryPoints: ["backend/main.ts"],
    outfile,
    bundle: true,
    platform: "node",
    target: "node22",
    format: "cjs",
    define: stopOnPageClose
      ? { "process.env.STOP_ON_PAGE_CLOSE": JSON.stringify(stopOnPageClose) }
      : undefined,
    plugins: [
      {
        name: "embedded-frontend",
        setup(b) {
          b.onResolve({ filter: /^\.\/embedded$/ }, () => ({
            path: path.resolve("build/embedded.ts"),
          }));
        },
      },
    ],
  });
}
