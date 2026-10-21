import test from "node:test";
import assert from "node:assert/strict";
import { mkdtemp, readFile, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import path from "node:path";
import {
  parseSprite,
  parseSpriteAny,
  serializeSprite,
  serializeSpriteYaml,
} from "../backend/format";
import { Workspace } from "../backend/workspace";
import { validateSprite } from "../shared/sprite";

const legacy = `/*SPRITE*/ {
VERSION 0001
IMAGE 0x0000 ".\\\\a.tga" TRANSP 0x00FF00FF
MODULES
{
MD 0x1000 MD_IMAGE 0 3 5 7 7 "m"
MD 0x1001 MD_FILL_RECT 255 4 4
}
FRAME "f"
{
0x2000
RC 0 0 4 4
FM 0x1000 -13 7 +FLIP_X
FM 0x1001 1 2 FLAGS 0x20
}
FRAME ""
{
0x2001
FM 0x2000 0 0 +HYPER_FM
}
ANIM "dizzy"
{
0x3000
AF 0x2000 2 0 0 +ROT_90
}
SPRITE_END
}
`;

test("sprite YAML round-trips the document model", () => {
  const doc = parseSprite(legacy);
  const yaml = serializeSpriteYaml(doc);
  assert.match(yaml, /id: "0x1000"/);
  assert.equal(JSON.stringify(parseSpriteAny(yaml)), JSON.stringify(doc));
  assert.deepEqual(parseSprite(serializeSprite(parseSpriteAny(yaml))), doc);
  assert.deepEqual(validateSprite(parseSpriteAny(yaml)), []);
});

test("workspace opens and saves .yaml sprites", async () => {
  const root = await mkdtemp(path.join(tmpdir(), "auroramx-yaml-"));
  const workspace = new Workspace(root);
  const doc = parseSprite(legacy);
  await writeFile(path.join(root, "a.yaml"), serializeSpriteYaml(doc));
  const opened = await workspace.open("a.yaml");
  assert.deepEqual(opened.document, doc);
  const edited = structuredClone(opened.document);
  edited.modules[0].values[1] = 9;
  const saved = await workspace.save("a.yaml", edited, opened.revision);
  assert.equal(saved.document.modules[0].values[1], 9);
  assert.match(await readFile(path.join(root, "a.yaml"), "utf8"), /^sprite:/);
  await workspace.save("b.yaml", edited, "", true);
  assert.deepEqual((await workspace.open("b.yaml")).document, edited);
});
