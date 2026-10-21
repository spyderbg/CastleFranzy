import Fastify from "fastify";
import { randomBytes } from "node:crypto";
import { readFile } from "node:fs/promises";
import { spawn } from "node:child_process";
import path from "node:path";
import { Workspace } from "./workspace";
import { compileSprite } from "./compiler";
import { parseSpriteAny } from "./format";
import { previewImage } from "./image";
import { atomicWrite, batchExport } from "./batch";
import { embeddedFiles } from "./embedded";
import type { SpriteDocument } from "../shared/sprite";
import defaults from "../config/defaults.json";
import { PageLifetime } from "./page-lifetime";

export function createServer(
  workspace: Workspace,
  options: {
    dev?: boolean;
    frontendPort?: number;
    onShutdown?: () => void;
  } = {},
) {
  const app = Fastify({
    logger: false,
    bodyLimit: 16 * 1024 * 1024,
    forceCloseConnections: true,
  });
  const token = randomBytes(24).toString("hex");
  let job: {
    running: boolean;
    exitCode: number | null;
    log: string;
    started: string;
  } | null = null;
  const autoShutdown = !!options.onShutdown && !options.dev;
  const lifetime = new PageLifetime(
    () => options.onShutdown?.(),
    () => !!job?.running,
  );
  app.addHook("preClose", async () => lifetime.dispose());
  app.addHook("onRequest", async (req, reply) => {
    reply.header("Cache-Control", "no-store");
    reply.header("X-Content-Type-Options", "nosniff");
    reply.header(
      "Content-Security-Policy",
      "frame-ancestors 'none'; base-uri 'self'",
    );
    if (!["127.0.0.1", "localhost", "[::1]"].includes(req.hostname))
      return reply.code(403).send({ error: "Local access only" });
    if (
      req.url.startsWith("/api/") &&
      (req.headers["sec-fetch-site"] === "cross-site" ||
        (!["GET", "HEAD"].includes(req.method) &&
          req.headers["x-auroramx-token"] !== token))
    )
      return reply
        .code(403)
        .send({ error: "Reload AuroraMX to reconnect to this session" });
  });
  app.setErrorHandler((error, req, reply) => {
    reply
      .code((error as { statusCode?: number }).statusCode || 400)
      .send({ error: (error as Error).message });
  });
  app.get("/api/session", async (_req, reply) => {
    reply.header(
      "Set-Cookie",
      `auroramx-session=${token}; HttpOnly; SameSite=Strict; Path=/api/lifecycle`,
    );
    return {
      token,
      root: workspace.root,
      app: "AuroraMX",
      version: "1.0.0",
      development: !!options.dev,
      autoShutdownOnClose: autoShutdown,
    };
  });
  app.get("/api/lifecycle", async (req, reply) => {
    if (!autoShutdown)
      return reply.code(503).send({ error: "Automatic shutdown is disabled" });
    const authenticated = req.headers.cookie
      ?.split(";")
      .some((cookie) => cookie.trim() === `auroramx-session=${token}`);
    if (!authenticated)
      return reply.code(403).send({ error: "Reload AuroraMX to reconnect" });
    reply.hijack();
    lifetime.connect(reply.raw);
  });
  app.get<{ Querystring: { directory?: string } }>("/api/files", async (req) =>
    workspace.list(req.query.directory || defaults.spriteDirectory),
  );
  app.get<{ Querystring: { path: string } }>("/api/sprite", async (req) =>
    workspace.open(req.query.path),
  );
  app.post<{
    Body: {
      path: string;
      document: SpriteDocument;
      revision: string;
      saveAs?: boolean;
      sourcePath?: string;
      rawSource?: string;
    };
  }>("/api/save", async (req) =>
    workspace.save(
      req.body.path,
      req.body.document,
      req.body.revision,
      req.body.saveAs,
      req.body.sourcePath,
      req.body.rawSource,
    ),
  );
  app.post<{ Body: { source: string } }>("/api/parse", async (req) => ({
    document: parseSpriteAny(req.body.source),
  }));
  app.get<{
    Querystring: {
      sprite: string;
      file: string;
      transparent?: string;
      alpha?: string;
    };
  }>("/api/image", async (req, reply) => {
    const q = req.query;
    const file = await workspace.imagePath(q.sprite, q.file);
    const bytes = await readFile(file);
    if (bytes.length > 64 * 1024 * 1024)
      throw new Error("Image preview is limited to 64 MB");
    const alpha = q.alpha
      ? await readFile(await workspace.imagePath(q.sprite, q.alpha))
      : undefined;
    return reply
      .type("image/png")
      .send(
        previewImage(
          bytes,
          q.transparent === undefined ? undefined : Number(q.transparent),
          alpha,
        ),
      );
  });
  app.post<{
    Body: { path: string; document: SpriteDocument; output?: string };
  }>("/api/export", async (req, reply) => {
    const bytes = compileSprite(req.body.document);
    if (req.body.output) {
      if (!req.body.output.toLowerCase().endsWith(".bsprite"))
        throw new Error("Binary output must end with .bsprite");
      await atomicWrite(await workspace.file(req.body.output, false), bytes);
      return { path: req.body.output, bytes: bytes.length };
    }
    return reply
      .type("application/octet-stream")
      .header(
        "Content-Disposition",
        `attachment; filename="${path.basename(req.body.path).replace(/\.(sprite|ya?ml)$/i, "")}.bsprite"`,
      )
      .send(bytes);
  });
  app.post<{ Body: { path: string; output?: string } }>(
    "/api/batch",
    async (req) => {
      const file = await workspace.file(req.body.path);
      if (!file.endsWith(".sprcmd")) throw new Error("Choose a .sprcmd file");
      const output = req.body.output
        ? await workspace.file(req.body.output, false)
        : path.join(workspace.root, "build/auroramx-export");
      // Batch exports always stay in a dedicated directory chosen in the workspace.
      return batchExport(file, output);
    },
  );
  app.get("/api/build", async () => job);
  app.post("/api/build", async () => {
    if (job?.running) throw new Error("A resource build is already running");
    const script = await workspace.file("rebuild_Linux.sh");
    job = {
      running: true,
      exitCode: null,
      log: "",
      started: new Date().toISOString(),
    };
    const current = job;
    const child = spawn("bash", [script, "--resources-only"], {
      cwd: workspace.root,
      env: process.env,
    });
    const append = (chunk: Buffer) => {
      current.log = (current.log + chunk.toString()).slice(-200000);
    };
    child.stdout.on("data", append);
    child.stderr.on("data", append);
    child.on("error", (error) => {
      current.log += error.message;
      current.running = false;
      current.exitCode = 1;
    });
    child.on("exit", (code) => {
      current.running = false;
      current.exitCode = code ?? 1;
    });
    return current;
  });
  app.post("/api/shutdown", async () => {
    if (job?.running) throw new Error("Wait for the resource build to finish");
    setTimeout(() => options.onShutdown?.(), 100);
    return { stopping: true };
  });
  app.setNotFoundHandler((req, reply) => {
    if (req.url.startsWith("/api/"))
      return reply.code(404).send({ error: "API endpoint not found" });
    if (options.dev)
      return reply.redirect(
        `http://127.0.0.1:${options.frontendPort || defaults.port + 1}`,
      );
    const pathname = req.url.split("?")[0];
    const file =
      embeddedFiles[pathname] ||
      (!path.extname(pathname) ? embeddedFiles["/index.html"] : undefined);
    if (!file)
      return reply
        .code(404)
        .send({ error: "Frontend is not built. Run npm run build." });
    return reply.type(file.mime).send(Buffer.from(file.base64, "base64"));
  });
  return app;
}
