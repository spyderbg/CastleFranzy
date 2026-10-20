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
