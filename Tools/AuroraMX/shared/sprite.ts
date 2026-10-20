export interface SpriteImage {
  id: number;
  file: string;
  alphaFile?: string;
  transparent?: number;
}
export interface SpriteModule {
  id: number;
  type: string;
  values: number[];
  name: string;
}
export interface Placement {
  target: number;
  x: number;
  y: number;
  flags: number;
}
export interface SpriteRect {
  x1: number;
  y1: number;
  x2: number;
  y2: number;
}
export interface SpriteFrame {
  id: number;
  name: string;
  rects: SpriteRect[];
  parts: Placement[];
}
export interface AnimationFrame extends Placement {
  time: number;
}
export interface SpriteAnimation {
  id: number;
  name: string;
  frames: AnimationFrame[];
}
export interface SpriteDocument {
  version: string;
  images: SpriteImage[];
  modules: SpriteModule[];
  frames: SpriteFrame[];
  animations: SpriteAnimation[];
  extras: string[];
}
export interface OpenSprite {
  path: string;
  document: SpriteDocument;
  revision: string;
  source: string;
}
export const FLAGS = { FLIP_X: 1, FLIP_Y: 2, ROT_90: 4, HYPER_FM: 16 } as const;
export const hex = (n: number) =>
  "0x" + n.toString(16).toUpperCase().padStart(4, "0");
export function emptySprite(): SpriteDocument {
  return {
    version: "0001",
    images: [],
    modules: [],
    frames: [],
    animations: [],
    extras: [],
  };
}
export function nextId(items: { id: number }[], base: number) {
  const ids = new Set(items.map((v) => v.id));
  while (ids.has(base)) base++;
  return base;
}
export function moduleSize(module: SpriteModule): [number, number] {
  if (module.type === "MD_IMAGE") return [module.values[3], module.values[4]];
  if (module.type === "MD_MARKER") return [module.values[0], module.values[1]];
  return [module.values[1], module.values[2]];
}
export function frameBounds(
  sprite: SpriteDocument,
  frame: SpriteFrame,
  visiting = new Set<number>(),
): SpriteRect {
  if (visiting.has(frame.id))
    throw new Error(`Cyclic hyperframe ${hex(frame.id)}`);
  const seen = new Set(visiting).add(frame.id);
  const boxes = frame.parts.map((p) => {
    if (p.flags & FLAGS.HYPER_FM) {
      const child = sprite.frames.find((f) => f.id === p.target);
      if (!child) throw new Error(`Missing hyperframe ${hex(p.target)}`);
      const r = frameBounds(sprite, child, seen);
      let points = [
        [r.x1, r.y1],
        [r.x2, r.y1],
        [r.x1, r.y2],
        [r.x2, r.y2],
      ];
      points = points.map(([x, y]) => {
        if (p.flags & 1) x = -x;
        if (p.flags & 2) y = -y;
        if (p.flags & 4) [x, y] = [-y, x];
        return [x + p.x, y + p.y];
      });
      return {
        x1: Math.min(...points.map((v) => v[0])),
        y1: Math.min(...points.map((v) => v[1])),
        x2: Math.max(...points.map((v) => v[0])),
        y2: Math.max(...points.map((v) => v[1])),
      };
    }
    const module = sprite.modules.find((m) => m.id === p.target);
    if (!module) throw new Error(`Missing module ${hex(p.target)}`);
    let [w, h] = moduleSize(module);
    if (p.flags & FLAGS.ROT_90) [w, h] = [h, w];
    return { x1: p.x, y1: p.y, x2: p.x + w, y2: p.y + h };
  });
  return boxes.length
    ? {
        x1: Math.min(...boxes.map((r) => r.x1)),
        y1: Math.min(...boxes.map((r) => r.y1)),
        x2: Math.max(...boxes.map((r) => r.x2)),
        y2: Math.max(...boxes.map((r) => r.y2)),
      }
    : { x1: 0, y1: 0, x2: 0, y2: 0 };
}
export function validateSprite(s: SpriteDocument): string[] {
  const errors: string[] = [];
  for (const [label, items] of [
    ["Image", s.images],
    ["Module", s.modules],
    ["Frame", s.frames],
    ["Animation", s.animations],
  ] as const) {
    const ids = new Set<number>();
    for (const item of items) {
      if (!Number.isInteger(item.id) || item.id < 0 || item.id > 65535)
        errors.push(`${label} has an invalid ID`);
      if (ids.has(item.id))
        errors.push(`Duplicate ${label.toLowerCase()} ID ${hex(item.id)}`);
      ids.add(item.id);
    }
  }
  for (const m of s.modules) {
    if (m.values.some((v) => !Number.isFinite(v) || !Number.isInteger(v)))
      errors.push(`${hex(m.id)} has invalid module values`);
    if (m.type === "MD_IMAGE" && m.values.length !== 5)
      errors.push(`${hex(m.id)} needs image, x, y, width, height`);
    if (m.type === "MD_IMAGE" && m.values[0] >= s.images.length)
      errors.push(`${hex(m.id)} references a missing image`);
    const [w, h] = moduleSize(m);
    if (!(w >= 0 && h >= 0)) errors.push(`${hex(m.id)} has an invalid size`);
  }
  for (const f of s.frames) {
    for (const p of f.parts)
      if (![p.x, p.y, p.target, p.flags].every(Number.isInteger))
        errors.push(`Invalid placement in ${hex(f.id)}`);
    for (const r of f.rects)
      if (
        ![r.x1, r.y1, r.x2, r.y2].every(Number.isInteger) ||
        r.x2 < r.x1 ||
        r.y2 < r.y1
      )
        errors.push(`Invalid collision rectangle in ${hex(f.id)}`);
    try {
      frameBounds(s, f);
    } catch (e) {
      errors.push((e as Error).message);
    }
  }
  for (const a of s.animations)
    for (const p of a.frames) {
      if (!s.frames.some((f) => f.id === p.target))
        errors.push(`${hex(a.id)} references missing frame ${hex(p.target)}`);
      if (![p.x, p.y, p.time, p.flags].every(Number.isInteger) || p.time < 0)
        errors.push(`Invalid animation step in ${hex(a.id)}`);
    }
  return [...new Set(errors)];
}
