import { test, expect } from "@playwright/test";
import { mkdir, rm, readFile } from "node:fs/promises";
import path from "node:path";
import { parseSprite } from "../../backend/format";
const root = path.resolve("../..");
const dir = "build/auroramx-ui-tests";
test.beforeAll(async () => {
  await rm(path.join(root, dir), { recursive: true, force: true });
  await mkdir(path.join(root, dir), { recursive: true });
});
test("loads classic editor, edits a frame, undoes/redoes, saves/reopens and exports a changed binary", async ({
  page,
}) => {
  const errors: string[] = [];
  page.on("pageerror", (error) => errors.push(error.message));
  await page.goto("/?file=Data/Sprites/Menu/menu_bg.sprite");
  await expect(
    page.getByRole("table", { name: "Modules", exact: true }),
  ).toBeVisible();
  await page.getByRole("tab", { name: "Frame Editor", exact: true }).click();
  await page
    .getByRole("table", { name: "Frames", exact: true })
    .getByRole("row")
    .filter({ hasText: "slider_fill" })
    .click();
  await page
    .getByRole("table", { name: "FModules", exact: true })
    .getByRole("row")
    .nth(1)
    .click();
  const x = page.getByRole("spinbutton", { name: "Offset X", exact: true });
  await x.fill("-120");
  await x.press("Enter");
  await page.getByRole("button", { name: "Undo", exact: true }).click();
  await expect(x).toHaveValue("-123");
  await page.getByRole("button", { name: "Redo", exact: true }).click();
  await expect(x).toHaveValue("-120");
  await page.getByRole("checkbox", { name: "Flip X", exact: true }).check();
  const preview = (await page
    .getByLabel("Sprite preview", { exact: true })
    .boundingBox())!;
  await page.mouse.move(
    preview.x + preview.width / 2 - 115,
    preview.y + preview.height / 2 - 4,
  );
  await page.mouse.down();
  await page.mouse.move(
    preview.x + preview.width / 2 - 107,
    preview.y + preview.height / 2 + 1,
    { steps: 5 },
  );
  await page.mouse.up();
  await expect(x).toHaveValue("-112");
  await page.keyboard.press("Control+Shift+S");
  await page
    .getByRole("textbox", { name: "Save file path" })
    .fill(dir + "/edited.sprite");
  await page.getByRole("button", { name: "Save", exact: true }).click();
  await expect(page.getByRole("dialog")).toHaveCount(0);
  const saved = parseSprite(
    await readFile(path.join(root, dir, "edited.sprite"), "utf8"),
  );
  expect(saved.frames[3].parts[0].x).toBe(-112);
  expect(saved.frames[3].parts[0].flags & 1).toBe(1);
  expect(saved.images[0].file).toContain("Data/Sprites/Menu/menu_bg.tga");
  await page.reload();
  await page.getByRole("button", { name: "Open sprite", exact: true }).click();
  await page
    .getByRole("textbox", { name: "Open file path" })
    .fill(dir + "/edited.sprite");
  await page.getByRole("button", { name: "Open", exact: true }).click();
  await page.getByRole("tab", { name: "Source Editor", exact: true }).click();
  const source = page.getByRole("textbox", { name: "Sprite source" });
  await source.fill(
    "// Edited in AuroraMX source editor\n" + (await source.inputValue()),
  );
  await page.getByRole("button", { name: "Apply source", exact: true }).click();
  await page.getByRole("tab", { name: "Frame Editor", exact: true }).click();
  await page.getByRole("tab", { name: "Source Editor", exact: true }).click();
  await expect(source).toHaveValue(/Edited in AuroraMX source editor/);
  await page.getByRole("tab", { name: "Frame Editor", exact: true }).click();
  await page.getByRole("button", { name: "Save sprite", exact: true }).click();
  await expect
    .poll(() => readFile(path.join(root, dir, "edited.sprite"), "utf8"))
    .toContain("Edited in AuroraMX source editor");
  await page.getByRole("tab", { name: "Frame Editor", exact: true }).click();
  await page
    .getByRole("button", { name: "Export binary sprite", exact: true })
    .click();
  await page
    .getByRole("textbox", { name: "Binary output path" })
    .fill(dir + "/edited.bsprite");
  await page
    .getByRole("button", { name: "Export .bsprite", exact: true })
    .click();
  await expect(page.getByRole("dialog")).toHaveCount(0);
  const binary = await readFile(path.join(root, dir, "edited.bsprite"));
  expect(binary.readUInt16LE(0)).toBe(0x05df);
  expect(
    binary.equals(
      await readFile(path.join(root, "Data/Sprites/bin/Menu/menu_bg.bsprite")),
    ),
  ).toBe(false);
  await expect(page.getByRole("alert")).toHaveCount(0);
  expect(errors).toEqual([]);
  await page.screenshot({
    path: path.join(root, "build/verification/auroramx-frame-editor.png"),
  });
});

test("Build game data runs the AuroraMX resource pipeline and reports success in the browser", async ({
  page,
}) => {
  test.setTimeout(120000);
  await page.goto("/");
  await expect(
    page.getByRole("table", { name: "Modules", exact: true }),
  ).toBeVisible();
  await page
    .getByRole("button", { name: "Build game data", exact: true })
    .click();
  await expect(
    page.getByRole("dialog", { name: "Build game data" }),
  ).toBeVisible();
  await expect(
    page.getByText("Game resources built successfully.", { exact: true }),
  ).toBeVisible({ timeout: 90000 });
  const log = page.getByRole("log");
  await expect(log).toContainText("AuroraMX exported 29 sprites");
  await expect(log).toContainText("Rebuilt sprites with AuroraMX");
  await page.screenshot({
    path: path.join(root, "build/verification/auroramx-resource-build.png"),
  });
});
test("creates a sprite with image, drawn module, composed frame, collision rectangle and playable animation", async ({
  page,
}) => {
  await page.goto("/");
  await expect(
    page.getByRole("table", { name: "Modules", exact: true }),
  ).toBeVisible();
  await page.getByRole("button", { name: "New sprite", exact: true }).click();
  await page
    .getByRole("textbox", { name: "Save file path" })
    .fill(dir + "/new.sprite");
  await page.getByRole("button", { name: "Create", exact: true }).click();
  const secondary = page.locator(".secondary-panel");
  await secondary.getByRole("button", { name: "Insert", exact: true }).click();
  const image = page.getByRole("textbox", { name: "Image file", exact: true });
  await image.fill("../../Data/Sprites/Menu/menu_bg.tga");
  await image.press("Enter");
  await page.waitForFunction(
    () =>
      document.querySelector(".image-tile img") instanceof HTMLImageElement &&
      (document.querySelector(".image-tile img") as HTMLImageElement)
        .naturalWidth > 0,
  );
  await page.getByRole("button", { name: "Draw module on atlas" }).click();
  const bounds = (await page
    .getByLabel("Sprite preview", { exact: true })
    .boundingBox())!;
  const left = bounds.x + (bounds.width - 512) / 2,
    top = bounds.y + (bounds.height - 512) / 2;
  await page.mouse.move(left + 20, top + 20);
  await page.mouse.down();
  await page.mouse.move(left + 60, top + 50, { steps: 5 });
  await page.mouse.up();
  await expect(
    page.getByRole("spinbutton", { name: "Width", exact: true }),
  ).toHaveValue("40");
  await expect(
    page.getByRole("spinbutton", { name: "Height", exact: true }),
  ).toHaveValue("30");
  await page.getByRole("tab", { name: "Frame Editor", exact: true }).click();
  await page
    .locator(".primary-panel")
    .getByRole("button", { name: "Insert", exact: true })
    .click();
  await page.locator(".bank .thumb").first().dblclick();
  await page
    .getByRole("button", { name: "+ Add rectangle", exact: true })
    .click();
  await expect(page.getByRole("spinbutton", { name: "RC 0 x1" })).toHaveValue(
    "-16",
  );
  await page
    .getByRole("tab", { name: "Animation Editor", exact: true })
    .click();
  await page
    .locator(".primary-panel")
    .getByRole("button", { name: "Insert", exact: true })
    .click();
  await page.locator(".bank .thumb").first().dblclick();
  await page
    .locator(".secondary-panel")
    .getByRole("button", { name: "Clone", exact: true })
    .click();
  await expect(
    page.getByRole("spinbutton", { name: "Time", exact: true }),
  ).toHaveValue("1");
  await page
    .getByRole("button", { name: "Play animation", exact: true })
    .click();
  await expect(
    page.getByRole("button", { name: "Pause animation", exact: true }),
  ).toBeVisible();
  await page
    .getByRole("button", { name: "Pause animation", exact: true })
    .click();
  await page.getByRole("button", { name: "Save sprite", exact: true }).click();
  await page.getByRole("button", { name: "Save", exact: true }).click();
  await expect(page.getByRole("dialog")).toHaveCount(0);
  const saved = parseSprite(
    await readFile(path.join(root, dir, "new.sprite"), "utf8"),
  );
  expect(saved.modules).toHaveLength(1);
  expect(saved.frames[0].parts).toHaveLength(1);
  expect(saved.frames[0].rects).toHaveLength(1);
  expect(saved.animations[0].frames).toHaveLength(2);
  await page.screenshot({
    path: path.join(root, "build/verification/auroramx-animation-editor.png"),
  });
});
test("runs the real Sprites.sprcmd from the browser and shows every exported sprite", async ({
  page,
}) => {
  await page.goto("/");
  await expect(
    page.getByRole("table", { name: "Modules", exact: true }),
  ).toBeVisible();
  await page.getByRole("button", { name: "File", exact: true }).click();
  await page
    .getByRole("button", { name: "Run sprite commands…", exact: true })
    .click();
  await page
    .getByRole("textbox", { name: "Batch output directory" })
    .fill(dir + "/batch");
  await page.getByRole("button", { name: "Run commands", exact: true }).click();
  await expect(
    page.getByText("Exported 29 sprites.", { exact: false }).first(),
  ).toBeVisible();
  expect(
    (await readFile(path.join(root, dir, "batch/bin/Menu/menu_bg.bsprite")))
      .length,
  ).toBe(349);
  await expect(page.getByRole("alert")).toHaveCount(0);
});
