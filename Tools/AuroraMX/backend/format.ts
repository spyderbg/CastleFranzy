import { parse as parseYamlText, stringify as stringifyYaml } from "yaml";
import {
  emptySprite,
  FLAGS,
  hex,
  type SpriteDocument,
  type Placement,
} from "../shared/sprite";

interface Token {
  value: string;
  quoted: boolean;
  line: number;
  start: number;
  end: number;
}
function tokens(text: string): Token[] {
  const result: Token[] = [];
  const pattern =
    /\/\/[^\r\n]*|\/\*[\s\S]*?\*\/|"(?:\\"|[^"\r\n])*"|[{}]|[^\s{}"]+/g;
  let match;
  let line = 1;
  let last = 0;
  while ((match = pattern.exec(text))) {
    line += (text.slice(last, match.index).match(/\n/g) || []).length;
    last = match.index;
    const raw = match[0];
    if (raw.startsWith("//") || raw.startsWith("/*")) continue;
    const quoted = raw.startsWith('"');
    result.push({
      value: quoted ? raw.slice(1, -1).replace(/\\"/g, '"') : raw,
      quoted,
      line,
      start: match.index,
      end: pattern.lastIndex,
    });
  }
  return result;
}
export function parseSprite(text: string): SpriteDocument {
  const stream = tokens(text);
  let index = 0;
  const s = emptySprite();
  const take = () => {
    const t = stream[index++];
    if (!t) throw new Error("Unexpected end of .sprite file");
    return t;
  };
  const expect = (v: string) => {
    const t = take();
    if (t.value !== v)
      throw new Error(`Line ${t.line}: expected ${v}, found ${t.value}`);
  };
  const number = () => {
    const t = take();
    const n = Number(t.value);
    if (!Number.isFinite(n) || !Number.isInteger(n))
      throw new Error(`Line ${t.line}: expected an integer, found ${t.value}`);
    return n;
  };
  const placement = (count: number) => {
    const first = take();
    const line = first.line;
    index--;
    const values = Array.from({ length: count }, number);
    let flags = 0;
    while (stream[index]?.line === line && stream[index].value !== "}") {
      const flag = take().value;
      if (flag === "FLAGS") flags |= number();
      else if (flag.startsWith("+") && flag.slice(1) in FLAGS)
        flags |= FLAGS[flag.slice(1) as keyof typeof FLAGS];
      else throw new Error(`Line ${line}: unsupported placement flag ${flag}`);
    }
    return { values, flags };
  };
  if (stream[0]?.value === "SPRITE") index++;
  expect("{");
  while (index < stream.length && stream[index].value !== "}") {
    const keyword = take();
    switch (keyword.value) {
      case "VERSION":
        s.version = take().value;
        break;
      case "IMAGE": {
        const id = number();
        const file = take();
        if (!file.quoted)
          throw new Error(`Line ${file.line}: expected image filename`);
        const image: SpriteDocument["images"][number] = {
          id,
          file: file.value,
        };
        while (
          stream[index]?.line === keyword.line &&
          stream[index].value !== "}"
        ) {
          const option = take().value;
          if (option === "ALPHA") image.alphaFile = take().value;
          else if (option === "TRANSP") image.transparent = number();
          else
            throw new Error(
              `Line ${keyword.line}: unknown image option ${option}`,
            );
        }
        s.images.push(image);
        break;
      }
      case "MODULES": {
        expect("{");
        while (stream[index]?.value !== "}") {
          expect("MD");
          const id = number();
          const type = take().value;
          const sizes: Record<string, number> = {
            MD_IMAGE: 5,
            MD_RECT: 3,
            MD_FILL_RECT: 3,
            MD_MARKER: 2,
            MD_ARC: 5,
            MD_FILL_ARC: 5,
            MD_TRIANGLE: 5,
            MD_FILL_TRIANGLE: 5,
          };
          if (!(type in sizes)) throw new Error(`Unknown module type ${type}`);
          const values = Array.from({ length: sizes[type] }, number);
          const name = stream[index]?.quoted ? take().value : "";
          s.modules.push({ id, type, values, name });
        }
        expect("}");
        break;
      }
      case "FRAME": {
        const name = stream[index]?.quoted ? take().value : "";
        expect("{");
        const id = number();
        const frame: SpriteDocument["frames"][number] = {
          id,
          name,
          rects: [],
          parts: [],
        };
        while (stream[index]?.value !== "}") {
          const what = take();
          if (what.value === "RC") {
            const [x1, y1, x2, y2] = Array.from({ length: 4 }, number);
            frame.rects.push({ x1, y1, x2, y2 });
          } else if (what.value === "FM") {
            const {
              values: [target, x, y],
              flags,
            } = placement(3);
            frame.parts.push({ target, x, y, flags });
          } else
            throw new Error(
              `Line ${what.line}: unknown frame statement ${what.value}`,
            );
        }
        expect("}");
        s.frames.push(frame);
        break;
      }
      case "ANIM": {
        const name = stream[index]?.quoted ? take().value : "";
        expect("{");
        const id = number();
        const animation: SpriteDocument["animations"][number] = {
          id,
          name,
          frames: [],
        };
        while (stream[index]?.value !== "}") {
          expect("AF");
          const {
            values: [target, time, x, y],
            flags,
          } = placement(4);
          animation.frames.push({ target, time, x, y, flags });
        }
        expect("}");
        s.animations.push(animation);
        break;
      }
      case "SPRITE_END":
        break;
      default: {
        // Keep unrecognized top-level editor extensions intact when saving.
        let end = keyword.end;
        while (
          stream[index] &&
          stream[index].line === keyword.line &&
          !["{", "}"].includes(stream[index].value)
        )
          end = take().end;
        if (stream[index]?.value === "{") {
          let depth = 0;
          do {
            const t = take();
            if (t.value === "{") depth++;
            if (t.value === "}") depth--;
            end = t.end;
          } while (depth && index < stream.length);
          if (depth)
            throw new Error(
              `Line ${keyword.line}: unterminated extension block`,
            );
        }
        s.extras.push(text.slice(keyword.start, end));
      }
    }
  }
  expect("}");
  if (index !== stream.length)
    throw new Error(
      `Line ${stream[index].line}: unexpected content after the sprite`,
    );
  return s;
}
const quote = (s: string) => '"' + s.replace(/"/g, '\\"') + '"';
function flags(p: Placement) {
  return p.flags ? ` FLAGS 0x${p.flags.toString(16).toUpperCase()}` : "";
}
export function serializeSprite(s: SpriteDocument): string {
  const lines = [
    "// Saved by AuroraMX",
    "/*SPRITE*/ {",
    `    VERSION ${s.version}`,
    "",
  ];
  for (const i of s.images)
    lines.push(
      `    IMAGE ${hex(i.id)} ${quote(i.file)}${i.alphaFile ? " ALPHA " + quote(i.alphaFile) : ""}${i.transparent !== undefined ? " TRANSP 0x" + i.transparent.toString(16).toUpperCase().padStart(8, "0") : ""}`,
    );
  lines.push("", "    MODULES", "    {");
  for (const m of s.modules)
    lines.push(
      `        MD ${hex(m.id)} ${m.type} ${m.values.join(" ")}${m.name ? " " + quote(m.name) : ""}`,
    );
  lines.push("    }", "");
  for (const f of s.frames) {
    lines.push(`    FRAME ${quote(f.name)}`, "    {", `        ${hex(f.id)}`);
    for (const r of f.rects)
      lines.push(`        RC ${r.x1} ${r.y1} ${r.x2} ${r.y2}`);
    for (const p of f.parts)
      lines.push(`        FM ${hex(p.target)} ${p.x} ${p.y}${flags(p)}`);
    lines.push("    }", "");
  }
  for (const a of s.animations) {
    lines.push(`    ANIM ${quote(a.name)}`, "    {", `        ${hex(a.id)}`);
    for (const p of a.frames)
      lines.push(
        `        AF ${hex(p.target)} ${p.time} ${p.x} ${p.y}${flags(p)}`,
      );
    lines.push("    }", "");
  }
  lines.push(...s.extras, "    SPRITE_END", "}", "");
  return lines.join("\n");
}
export function decodeText(buffer: Buffer): string {
  if (buffer[0] === 255 && buffer[1] === 254)
    return buffer.subarray(2).toString("utf16le");
  try {
    return new TextDecoder("utf-8", { fatal: true }).decode(buffer);
  } catch {
    return new TextDecoder("windows-1252").decode(buffer);
  }
}

const MODULE_FIELDS: Record<string, string[]> = {
  MD_IMAGE: ["image", "x", "y", "w", "h"],
  MD_RECT: ["color", "w", "h"],
  MD_FILL_RECT: ["color", "w", "h"],
  MD_MARKER: ["w", "h"],
  MD_ARC: ["color", "w", "h", "startAngle", "arcAngle"],
  MD_FILL_ARC: ["color", "w", "h", "startAngle", "arcAngle"],
  MD_TRIANGLE: ["color", "p2X", "p2Y", "p3X", "p3Y"],
  MD_FILL_TRIANGLE: ["color", "p2X", "p2Y", "p3X", "p3Y"],
};
const HEX_FIELDS = new Set(["image", "color"]);
const FLAG_NAMES = Object.entries(FLAGS) as [string, number][];

export const isYamlPath = (file: string) => /\.ya?ml$/i.test(file);
export const isSpritePath = (file: string) => /\.(sprite|ya?ml)$/i.test(file);

export function looksLikeYaml(text: string) {
  return /^sprite\s*:/m.test(text) && !/^\s*\/\*\s*SPRITE\s*\*\//m.test(text);
}
export function parseSpriteAny(text: string): SpriteDocument {
  return looksLikeYaml(text) ? parseSpriteYaml(text) : parseSprite(text);
}
export function serializeSpriteFor(file: string, s: SpriteDocument) {
  return isYamlPath(file) ? serializeSpriteYaml(s) : serializeSprite(s);
}

const hex8 = (n: number) =>
  "0x" + (n >>> 0).toString(16).toUpperCase().padStart(8, "0");
function flagList(flags: number) {
  const names = FLAG_NAMES.filter(([, bit]) => flags & bit).map(([n]) => n);
  const rest = flags & ~FLAG_NAMES.reduce((a, [, bit]) => a | bit, 0);
  return { flags: names.length ? names : undefined, extraFlags: rest || undefined };
}
function placementOut(p: Placement) {
  const { flags, extraFlags } = flagList(p.flags);
  return { x: p.x, y: p.y, flags, extraFlags };
}
export function serializeSpriteYaml(s: SpriteDocument): string {
  const doc = {
    sprite: {
      version: s.version,
      images: s.images.map((i) => ({
        id: hex(i.id),
        file: i.file,
        alphaFile: i.alphaFile,
        transparent:
          i.transparent !== undefined ? hex8(i.transparent) : undefined,
      })),
      modules: s.modules.map((m) => {
        const fields = MODULE_FIELDS[m.type];
        if (!fields) throw new Error(`Unknown module type ${m.type}`);
        const out: Record<string, unknown> = { id: hex(m.id), type: m.type };
        fields.forEach((f, i) => {
          out[f] =
            HEX_FIELDS.has(f) && m.values[i] >= 0 ? hex(m.values[i]) : m.values[i];
        });
        if (m.name) out.name = m.name;
        return out;
      }),
      frames: s.frames.map((f) => ({
        id: hex(f.id),
        name: f.name,
        rects: f.rects.length ? f.rects : undefined,
        // Hyperframes are a `module` placement with the HYPER_FM flag: the game
        // reads `module` for every frame part and selects the target by flag.
        parts: f.parts.map((p) => ({
          module: hex(p.target),
          ...placementOut(p),
        })),
      })),
      animations: s.animations.map((a) => ({
        id: hex(a.id),
        name: a.name,
        frames: a.frames.map((p) => ({
          frame: hex(p.target),
          time: p.time,
          ...placementOut(p),
        })),
      })),
      extensions: s.extras.length ? s.extras : undefined,
    },
  };
  return stringifyYaml(doc, { lineWidth: 0, defaultStringType: "PLAIN" })
    .replace(/^(\s+- )?(id|image|color): ('?)(0x[0-9A-Fa-f]+)\3$/gm, '$1$2: "$4"')
    .replace(/^(\s+(?:- )?)(module|frame|transparent): ('?)(0x[0-9A-Fa-f]+)\3$/gm, '$1$2: "$4"');
}

type Obj = Record<string, unknown>;
export function parseSpriteYaml(text: string): SpriteDocument {
  let root: unknown;
  try {
    root = parseYamlText(text, { intAsBigInt: false });
  } catch (e) {
    throw new Error(`Invalid YAML: ${(e as Error).message}`);
  }
  const sprite = (root as Obj | null)?.sprite as Obj | undefined;
  if (!sprite || typeof sprite !== "object")
    throw new Error("YAML sprite files need a top-level 'sprite' mapping");
  const num = (v: unknown, what: string) => {
    const n = typeof v === "string" && v.trim() ? Number(v) : v;
    if (typeof n !== "number" || !Number.isInteger(n))
      throw new Error(`Expected an integer for ${what}, found ${String(v)}`);
    return n;
  };
  const list = (v: unknown, what: string): Obj[] => {
    if (v === undefined || v === null) return [];
    if (!Array.isArray(v)) throw new Error(`'${what}' must be a list`);
    return v.map((item, i) => {
      if (!item || typeof item !== "object" || Array.isArray(item))
        throw new Error(`${what}[${i}] must be a mapping`);
      return item as Obj;
    });
  };
  const str = (v: unknown, what: string) => {
    if (typeof v !== "string") throw new Error(`Expected text for ${what}`);
    return v;
  };
  const placement = (o: Obj, target: unknown, what: string, hyper = false) => {
    let flags = hyper ? FLAGS.HYPER_FM : 0;
    const names = o.flags ?? [];
    if (!Array.isArray(names)) throw new Error(`${what}.flags must be a list`);
    for (const n of names) {
      if (!(n in FLAGS)) throw new Error(`${what}: unknown flag ${n}`);
      flags |= FLAGS[n as keyof typeof FLAGS];
    }
    if (o.extraFlags !== undefined) flags |= num(o.extraFlags, `${what}.extraFlags`);
    return {
      target: num(target, `${what} target`),
      x: num(o.x ?? 0, `${what}.x`),
      y: num(o.y ?? 0, `${what}.y`),
      flags,
    };
  };
  const s = emptySprite();
  if (sprite.version !== undefined) s.version = String(sprite.version);
  list(sprite.images, "images").forEach((o, i) => {
    const image: SpriteDocument["images"][number] = {
      id: num(o.id, `images[${i}].id`),
      file: str(o.file, `images[${i}].file`),
    };
    if (o.alphaFile !== undefined) image.alphaFile = str(o.alphaFile, "alphaFile");
    if (o.transparent !== undefined)
      image.transparent = num(o.transparent, `images[${i}].transparent`);
    s.images.push(image);
  });
  list(sprite.modules, "modules").forEach((o, i) => {
    const type = str(o.type, `modules[${i}].type`);
    const fields = MODULE_FIELDS[type];
    if (!fields) throw new Error(`Unknown module type ${type}`);
    s.modules.push({
      id: num(o.id, `modules[${i}].id`),
      type,
      values: fields.map((f) => num(o[f], `modules[${i}].${f}`)),
      name: o.name === undefined ? "" : str(o.name, "module name"),
    });
  });
  list(sprite.frames, "frames").forEach((o, i) => {
    const what = `frames[${i}]`;
    s.frames.push({
      id: num(o.id, `${what}.id`),
      name: o.name === undefined ? "" : str(o.name, `${what}.name`),
      rects: list(o.rects, `${what}.rects`).map((r) => ({
        x1: num(r.x1, "rect x1"),
        y1: num(r.y1, "rect y1"),
        x2: num(r.x2, "rect x2"),
        y2: num(r.y2, "rect y2"),
      })),
      parts: list(o.parts, `${what}.parts`).map((p) =>
        p.frame !== undefined
          ? placement(p, p.frame, `${what}.parts`, true)
          : placement(p, p.module, `${what}.parts`),
      ),
    });
  });
  list(sprite.animations, "animations").forEach((o, i) => {
    const what = `animations[${i}]`;
    s.animations.push({
      id: num(o.id, `${what}.id`),
      name: o.name === undefined ? "" : str(o.name, `${what}.name`),
      frames: list(o.frames, `${what}.frames`).map((p) => {
        const { target, x, y, flags } = placement(p, p.frame, `${what}.frames`);
        return { target, time: num(p.time, `${what}.time`), x, y, flags };
      }),
    });
  });
  if (sprite.extensions !== undefined) {
    if (!Array.isArray(sprite.extensions))
      throw new Error("'extensions' must be a list of text blocks");
    s.extras = sprite.extensions.map((e) => str(e, "extension"));
  }
  return s;
}
