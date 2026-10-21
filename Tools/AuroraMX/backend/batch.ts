import { readFile, mkdir, writeFile, rename, unlink } from "node:fs/promises";
import path from "node:path";
import { createHash, randomUUID } from "node:crypto";
import { compileSprite, exportIds } from "./compiler";
import {
  parseSpriteAny,
  decodeText,
  serializeSpriteFor,
} from "./format";

export async function atomicWrite(file: string, data: string | Buffer) {
  await mkdir(path.dirname(file), { recursive: true });
  const temp = file + `.auroramx-${randomUUID()}.tmp`;
  try {
    await writeFile(temp, data, { flag: "wx" });
    await rename(temp, file);
  } finally {
    await unlink(temp).catch(() => {});
  }
}
export const normalizeTemplate = (text: string) =>
  text
    .replace(/\/\*[\s\S]*?\*\/|\/\/[^\r\n]*/g, "")
    .replace(/\s+/g, "")
    .toLowerCase();
export async function batchExport(commandFile: string, outputRoot?: string) {
  const base = path.dirname(path.resolve(commandFile));
  const source = decodeText(await readFile(commandFile)).replace(
    /\/\*[\s\S]*?\*\/|\/\/[^\r\n]*/g,
    "",
  );
  const commands = [...source.matchAll(/(\w+)\s*\(([^)]*)\)/g)];
  let sprite: ReturnType<typeof parseSpriteAny> | undefined;
  let initialized = false;
  const outputs = new Map<string, Buffer | string>();
  const report: { source: string; output: string; bytes: number }[] = [];
  let currentSource = "";
  const resolve = (p: string) => path.resolve(base, p.replace(/\\/g, "/"));
  const out = (p: string) => {
    const result = outputRoot
      ? path.resolve(outputRoot, p.replace(/\\/g, "/"))
      : resolve(p);
    const relative = outputRoot
      ? path.relative(path.resolve(outputRoot), result)
      : "";
    if (
      outputRoot &&
      (relative === ".." ||
        relative.startsWith(".." + path.sep) ||
        path.isAbsolute(relative))
    )
      throw new Error("Export path leaves the output directory");
    return result;
  };
  for (const command of commands) {
    const name = command[1];
    const args = [
      ...command[2].matchAll(/(?:^|,)\s*(?:"((?:\\"|[^"])*)"|([^,]*))/g),
    ].map((m) => m[1] ?? m[2].trim());
    if (name === "InitBSpriteFFT") {
      const template = normalizeTemplate(
        decodeText(await readFile(resolve(args[0]))),
      );
      const digest = createHash("sha256").update(template).digest("hex");
      if (digest !== GP_TEMPLATE_HASH)
        throw new Error(
          "This export uses the Castle Frenzy GP_export.fft profile. The selected FFT has different fields or options.",
        );
      initialized = true;
      continue;
    }
    if (name === "Load") {
      currentSource = resolve(args[0]);
      sprite = parseSpriteAny(decodeText(await readFile(currentSource)));
      continue;
    }
    if (!sprite) throw new Error(`${name} requires Load first`);
    if (name === "ExportBSpriteFFT") {
      if (!initialized)
        throw new Error("ExportBSpriteFFT requires InitBSpriteFFT first");
      const data = compileSprite(sprite);
      const target = out(args[0]);
      outputs.set(
        target,
        args[1]?.includes("APPEND")
          ? Buffer.concat([Buffer.from(outputs.get(target) || ""), data])
          : data,
      );
      report.push({
        source: currentSource,
        output: target,
        bytes: data.length,
      });
    } else if (
      ["ExportFramesID", "ExportAnimsID", "ExportModulesID"].includes(name)
    ) {
      const kind =
        name === "ExportFramesID"
          ? "frames"
          : name === "ExportAnimsID"
            ? "animations"
            : "modules";
      const target = out(args[0]);
      const value = exportIds(
        sprite,
        kind,
        args[1],
        args[2]?.includes("TOUPPER"),
      );
      outputs.set(
        target,
        (args[2]?.includes("APPEND") ? outputs.get(target) || "" : "") + value,
      );
    } else if (name === "Save")
      outputs.set(out(args[0]), serializeSpriteFor(out(args[0]), sprite));
    else throw new Error(`Unsupported sprite command ${name}`);
  }
  if (!report.length)
    throw new Error("No sprite exports found in command file");
  // Parse and validate the whole batch before replacing any output.
  for (const [file, data] of outputs) await atomicWrite(file, data);
  return { sprites: report, files: [...outputs.keys()] };
}
// Hash of the bundled GP_Data template with comments/whitespace removed.
// Arbitrary FFTs must not silently produce a binary with a different layout.
const GP_TEMPLATE_HASH =
  "3c262255ecf95dfe3186b2797caec52f454862a7e53522be4340eb0a26aff76f";
