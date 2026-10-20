import {
  frameBounds,
  validateSprite,
  type SpriteDocument,
} from "../shared/sprite";

class Writer {
  parts: Buffer[] = [];
  integer(n: number, bytes: number, signed = false, label = "value") {
    const min = signed ? -(2 ** (bytes * 8 - 1)) : 0,
      max = signed ? 2 ** (bytes * 8 - 1) - 1 : 2 ** (bytes * 8) - 1;
    if (!Number.isInteger(n) || n < min || n > max)
      throw new Error(
        `${label} ${n} does not fit ${signed ? "signed" : "unsigned"} ${bytes * 8}-bit export field`,
      );
    const b = Buffer.alloc(bytes);
    if (signed) b.writeIntLE(n, 0, bytes);
    else b.writeUIntLE(n, 0, bytes);
    this.parts.push(b);
  }
  u8 = (n: number, label?: string) => this.integer(n, 1, false, label);
  u16 = (n: number, label?: string) => this.integer(n, 2, false, label);
  i8 = (n: number, label?: string) => this.integer(n, 1, true, label);
  i16 = (n: number, label?: string) => this.integer(n, 2, true, label);
}
export function compileSprite(s: SpriteDocument): Buffer {
  const errors = validateSprite(s);
  if (errors.length) throw new Error(errors.join("\n"));
  if (!s.modules.length)
    throw new Error("Add at least one module before exporting a game sprite");
  if (s.modules.some((m) => m.type === "MD_IMAGE" && m.values[0] > 0))
    throw new Error(
      "GP_Data exports a single external atlas. Combine the module images into image index 0 before exporting.",
    );
  const w = new Writer();
  w.u16(0x05df);
  w.integer(0x1853b, 4);
  w.u16(s.modules.length);
  for (const m of s.modules) {
    if (m.type !== "MD_IMAGE")
      throw new Error(
        "GP_export.fft supports game image modules; shape modules cannot be exported with this template",
      );
    w.u8(0);
    for (const v of m.values.slice(1))
      w.u16(v, `Module ${m.id} coordinate/size`);
  }
  const moduleIndexes = new Map(s.modules.map((m, i) => [m.id, i]));
  const frameIndexes = new Map(s.frames.map((f, i) => [f.id, i]));
  w.u16(s.frames.reduce((n, f) => n + f.parts.length, 0));
  for (const f of s.frames)
    for (const p of f.parts) {
      const index = (p.flags & 16 ? frameIndexes : moduleIndexes).get(
        p.target,
      )!;
      w.u16(index);
      w.i16(p.x);
      w.i16(p.y);
      w.u8(p.flags);
    }
  w.u16(s.frames.reduce((n, f) => n + f.rects.length, 0));
  for (const f of s.frames)
    for (const r of f.rects)
      for (const n of [r.x1, r.y1, r.x2 - r.x1, r.y2 - r.y1]) w.i16(n);
  w.u16(s.frames.length);
  let first = 0;
  for (const f of s.frames) {
    w.u16(f.parts.length);
    w.i16(first);
    w.u8(f.rects.length);
    first += f.parts.length;
  }
  for (const f of s.frames) {
    const r = frameBounds(s, f);
    for (const n of [r.x1, r.y1, r.x2 - r.x1, r.y2 - r.y1]) w.i16(n);
  }
  w.u16(s.animations.reduce((n, a) => n + a.frames.length, 0));
  for (const a of s.animations)
    for (const p of a.frames) {
      const index = frameIndexes.get(p.target)!;
      if (index > 1023)
        throw new Error("Game animation frames are limited to indices 0..1023");
      w.u8(index & 255);
      w.u8(p.time, "Animation time");
      w.i8(p.x, "Animation X offset");
      w.i8(p.y, "Animation Y offset");
      w.u8(p.flags | ((index >> 8) << 6));
    }
  w.u16(s.animations.length);
  first = 0;
  for (const a of s.animations) {
    w.u16(a.frames.length);
    w.i16(first);
    first += a.frames.length;
  }
  // GP flags do not embed images. The game reads its atlas as a separate file.
  return Buffer.concat(w.parts);
}
export function exportIds(
  s: SpriteDocument,
  kind: "frames" | "animations" | "modules",
  prefix: string,
  uppercase = true,
): string {
  const seen = new Set<string>();
  const lines = [`// AuroraMX — ${kind}`];
  s[kind].forEach((v, i) => {
    let name = (prefix + (v.name || String(i))).replace(/[^a-zA-Z0-9_]/g, "_");
    if (uppercase) name = name.toUpperCase();
    if (seen.has(name)) lines.push(`// Duplicate ID name: ${name} (${i})`);
    else {
      lines.push(`#define ${name} ${i}`);
      seen.add(name);
    }
  });
  return lines.join("\n") + "\n";
}
