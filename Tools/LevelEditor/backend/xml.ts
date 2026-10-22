import { XMLParser, XMLValidator } from "fast-xml-parser";
import {
  newLevel,
  type Day,
  type Loadout,
  type Project,
} from "../shared/level";
export function decode(bytes: Buffer): string {
  if (bytes[0] === 0xff && bytes[1] === 0xfe)
    return bytes.subarray(2).toString("utf16le");
  if (bytes[0] === 0xfe && bytes[1] === 0xff)
    return Buffer.from(bytes.subarray(2)).swap16().toString("utf16le");
  return bytes.toString("utf8").replace(/^\uFEFF/, "");
}
export const list = <T>(v: T | T[] | undefined): T[] =>
  v === undefined ? [] : Array.isArray(v) ? v : [v];
export function parse(bytes: Buffer): any {
  const source = decode(bytes);
  const valid = XMLValidator.validate(source);
  if (valid !== true) throw new Error(`Invalid level XML: ${valid.err.msg}`);
  return new XMLParser({
    ignoreAttributes: false,
    attributeNamePrefix: "@",
    parseAttributeValue: false,
    processEntities: true,
  }).parse(source);
}
const value = (node: any, name: string, fallback = ""): string =>
  String(node?.[name]?.["@value"] ?? fallback);
const num = (node: any, name: string, fallback = 0) =>
  Number(value(node, name, String(fallback)));
const bool = (node: any, name: string, fallback = false) =>
  ["true", "1"].includes(value(node, name, String(fallback)));
export function readInfo(bytes: Buffer): number[] {
  return list<any>(parse(bytes).levels?.level).map((n) => num(n, "days"));
}
export function readLoadout(bytes: Buffer): Loadout {
  const node = parse(bytes).level,
    result = newLevel().loadout;
  result.weapons.forEach((w, i) => {
    w.startingLevel = num(node, `weapon-${i + 1}-starting-level`, -1);
    w.shop = bool(node, `weapon-${i + 1}-available-in-shop`, true);
  });
  result.spell = num(node, "spell-available", -1);
  result.spellBought = bool(node, "spell-bought");
  return result;
}
export function readDay(bytes: Buffer): Day {
  const n = parse(bytes).level;
  if (!n) throw new Error("Expected <level>");
  return {
    dayEndTime: num(n, "dayEndTime", 100),
    difficulty: num(n, "difficulty"),
    challenge: value(n, "challenge", "None"),
    waves: list<any>(n.waves?.wave).map((w) => ({
      name: value(w, "name"),
      timeStart: num(w, "timeStart"),
      timeInterval: num(w, "timeInterval"),
      repeatInterval: num(w, "repeatInterval"),
      repeatCount: num(w, "repeatCount", 1),
      packed: bool(w, "packed"),
      variants: list<any>(w.variant).map((v) => ({
        intro: bool(v, "intro"),
        enemies: list<any>(v.enemy).map((e) => ({
          type: value(e, "type"),
          min: num(e, "min", 1),
          max: num(e, "max", 1),
        })),
      })),
    })),
  };
}
const escape = (v: unknown) =>
  String(v)
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&apos;");
const field = (name: string, v: unknown) => `<${name} value="${escape(v)}" />`;
const wrap = (name: string, fields: string[]) =>
  `<${name}>\n${fields
    .join("\n")
    .split("\n")
    .map((s) => "  " + s)
    .join("\n")}\n</${name}>`;
const xml = (body: string) =>
  Buffer.from(`<?xml version="1.0" encoding="utf-8"?>\n${body}\n`);
export const infoBytes = (project: Project) =>
  xml(
    wrap(
      "levels",
      project.levels.map((l) => wrap("level", [field("days", l.days.length)])),
    ),
  );
export const loadoutBytes = (l: Loadout) =>
  xml(
    wrap("level", [
      ...l.weapons.flatMap((w, i) => [
        field(`weapon-${i + 1}-starting-level`, w.startingLevel),
        field(`weapon-${i + 1}-available-in-shop`, w.shop),
      ]),
      field("spell-available", l.spell),
      field("spell-bought", l.spellBought),
    ]),
  );
export const dayBytes = (d: Day) =>
  xml(
    wrap("level", [
      field("dayEndTime", d.dayEndTime),
      field("challenge", d.challenge),
      field("difficulty", d.difficulty),
      wrap(
        "waves",
        d.waves.map((w) =>
          wrap("wave", [
            field("name", w.name),
            field("timeStart", w.timeStart),
            field("timeInterval", w.timeInterval),
            field("repeatInterval", w.repeatInterval),
            field("repeatCount", w.repeatCount),
            field("packed", w.packed),
            ...w.variants.map((v) =>
              wrap("variant", [
                field("intro", v.intro),
                ...v.enemies.map((e) =>
                  wrap("enemy", [
                    field("type", e.type),
                    field("min", e.min),
                    field("max", e.max),
                  ]),
                ),
              ]),
            ),
          ]),
        ),
      ),
    ]),
  );

/** The original Irrlicht config reader accepts multiple roots and incomplete empty tags. */
export function configEntries(
  bytes: Buffer,
  tag: string,
): Map<string, Record<string, string>> {
  const source = decode(bytes).replace(/<!--[\s\S]*?-->/g, "");
  const entries = new Map<
    string,
    { parent?: string; values: Record<string, string> }
  >();
  const attrs = (s: string) =>
    Object.fromEntries(
      [...s.matchAll(/([\w-]+)\s*=\s*"([^"]*)"/g)].map((m) => [
        m[1],
        m[2].replace(/&amp;/g, "&"),
      ]),
    );
  for (const m of source.matchAll(
    new RegExp(`<${tag}\\b([^>]*)>([\\s\\S]*?)<\\/${tag}>`, "g"),
  )) {
    const a = attrs(m[1]),
      values: Record<string, string> = {};
    for (const f of m[2].matchAll(
      /<(?:string|float|int|bool|rect)\b([^<]*)/g,
    )) {
      const a = attrs(f[1]);
      if (a.name) values[a.name] = a.value;
    }
    if (a.id) entries.set(a.id, { parent: a.extends, values });
  }
  const resolve = (
    name: string,
    seen = new Set<string>(),
  ): Record<string, string> => {
    if (seen.has(name)) throw new Error(`Cyclic config inheritance: ${name}`);
    seen.add(name);
    const e = entries.get(name);
    if (!e) throw new Error(`Unknown config parent: ${name}`);
    return { ...(e.parent ? resolve(e.parent, seen) : {}), ...e.values };
  };
  return new Map(
    [...entries.keys()].sort().map((name) => [name, resolve(name)]),
  );
}
