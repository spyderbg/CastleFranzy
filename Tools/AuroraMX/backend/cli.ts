import path from "node:path";
import { readFile } from "node:fs/promises";
import { batchExport, atomicWrite } from "./batch";
import { compileSprite } from "./compiler";
import { decodeText, parseSpriteAny, serializeSpriteFor } from "./format";

async function main() {
  const [command, input, ...args] = process.argv.slice(2);
  if (!input || !["batch", "export", "validate", "convert"].includes(command)) {
    console.log(
      "AuroraMX\nUsage:\n  auroramx-cli batch file.sprcmd [--output directory]\n  auroramx-cli export file.sprite|yaml output.bsprite\n  auroramx-cli validate file.sprite|yaml\n  auroramx-cli convert input.sprite|yaml output.yaml|sprite",
    );
    process.exitCode = 2;
    return;
  }
  if (command === "batch") {
    const position = args.indexOf("--output");
    const output = position >= 0 ? args[position + 1] : undefined;
    if (position >= 0 && !output)
      throw new Error("--output requires a directory");
    const report = await batchExport(
      path.resolve(input),
      output && path.resolve(output),
    );
    for (const sprite of report.sprites)
      console.log(
        `${sprite.source} -> ${sprite.output} (${sprite.bytes} bytes)`,
      );
    console.log(
      `AuroraMX prepared ${report.sprites.length} sprites and ${report.files.length - report.sprites.length} ID headers.`,
    );
  } else if (command === "convert") {
    if (!args[0]) throw new Error("Convert requires an output filename");
    const sprite = parseSpriteAny(decodeText(await readFile(input)));
    await atomicWrite(args[0], serializeSpriteFor(args[0], sprite));
    console.log(`${input} -> ${args[0]}`);
  } else {
    const sprite = parseSpriteAny(decodeText(await readFile(input)));
    const bytes = compileSprite(sprite);
    if (command === "export") {
      if (!args[0]) throw new Error("Export requires an output filename");
      await atomicWrite(args[0], bytes);
    }
    console.log(
      `${input}: ${sprite.modules.length} modules, ${sprite.frames.length} frames, ${sprite.animations.length} animations; ${bytes.length} binary bytes`,
    );
  }
}
main().catch((error) => {
  console.error(`AuroraMX: ${error.message}`);
  process.exitCode = 1;
});
