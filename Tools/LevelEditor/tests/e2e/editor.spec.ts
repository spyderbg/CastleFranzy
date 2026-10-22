import { test, expect } from "@playwright/test";
test("edit waves, variants and enemies; timeline drag, copy/paste, undo and save survive reload", async ({
  page,
}) => {
  await page.goto("/");
  await expect(
    page.getByRole("heading", { name: "Wave composition" }),
  ).toBeVisible();
  await page.getByLabel("Wave name", { exact: true }).fill("Browser test");
  await page.getByLabel("Wave start", { exact: true }).fill("3");
  await page
    .getByRole("button", { name: "+ New variant", exact: true })
    .click();
  await page
    .getByRole("button", { name: "+ New enemy type", exact: true })
    .last()
    .click();
  await page
    .getByLabel("Unit type 2-1", { exact: true })
    .selectOption("Goblin");
  await page.getByLabel("Min count 2-1").fill("4");
  await page.getByLabel("Max count 2-1").fill("7");
  const track = page.getByLabel("Timeline wave 1", { exact: true });
  const box = (await track.boundingBox())!;
  await page.mouse.move(box.x + 20, box.y + 15);
  await page.mouse.down();
  await page.mouse.move(box.x + 110, box.y + 15);
  await page.mouse.up();
  await expect(page.getByLabel("Wave start", { exact: true })).not.toHaveValue(
    "3",
  );
  await page.getByRole("button", { name: "Clone", exact: true }).click();
  await expect(page.getByLabel("Wave name", { exact: true })).toHaveValue(
    "Browser test",
  );
  await page.getByRole("button", { name: "Copy", exact: true }).click();
  await page.getByRole("button", { name: "Paste", exact: true }).click();
  await expect(page.getByText("Day 07", { exact: true })).toBeVisible();
  await page.getByRole("button", { name: "Undo", exact: true }).click();
  await expect(page.getByText("Day 07", { exact: true })).not.toBeVisible();
  await page.getByRole("button", { name: "Redo", exact: true }).click();
  await expect(page.getByText("Day 07", { exact: true })).toBeVisible();
  await page.keyboard.press("Control+s");
  await expect(page.getByText("All changes saved")).toBeVisible();
  await page.reload();
  await page.getByText("Day 07", { exact: true }).click();
  await expect(page.getByLabel("Wave name", { exact: true })).toHaveValue(
    "Browser test",
  );
  await page.screenshot({ path: "build/waves.png", fullPage: true });
});
test("weapons, spells, endless difficulty and empty collections", async ({
  page,
}) => {
  await page.goto("/");
  await page.getByRole("button", { name: /Weapons$/ }).click();
  await page
    .getByRole("button", { name: "Hydra level 3", exact: true })
    .click();
  await page
    .getByRole("button", { name: "Spell Tornado", exact: true })
    .click();
  await page.getByLabel("Spell bought", { exact: true }).check();
  await page.keyboard.press("Control+s");
  await expect(page.getByText("All changes saved")).toBeVisible();
  await page.reload();
  await page.getByRole("button", { name: /Weapons$/ }).click();
  await expect(
    page.getByRole("button", { name: "Hydra level 3", exact: true }),
  ).toHaveClass(/selected/);
  await expect(page.getByLabel("Spell bought")).toBeChecked();
  await page.screenshot({ path: "build/weapons.png", fullPage: true });
  await page.getByRole("button", { name: /Waves$/ }).click();
  await page.getByRole("button", { name: /Endless \d+ days/ }).click();
  await page.getByLabel("Difficulty", { exact: true }).selectOption("2");
  await expect(page.getByText("Hard 1", { exact: true })).toBeVisible();
  await page.getByRole("button", { name: "New level", exact: true }).click();
  await expect(
    page.getByRole("heading", { name: "Add a day to this level" }),
  ).toBeVisible();
  await page.getByRole("button", { name: "New day", exact: true }).click();
  await page.getByRole("button", { name: "+ New wave", exact: true }).click();
  await expect(page.getByLabel("Wave name", { exact: true })).toHaveValue(
    "wave1",
  );
});
test("all configured models render and expose their animation clips", async ({
  page,
}) => {
  page.on("console", (message) => {
    if (message.type() === "error") console.log(message.text());
  });
  const errors: string[] = [];
  page.on("pageerror", (e) => errors.push(e.message));
  await page.goto("/");
  await page.getByRole("button", { name: /Models$/ }).click();
  await expect(page.locator(".model-list button")).toHaveCount(21);
  const models = await page.locator(".model-list button").allTextContents();
  for (const model of models) {
    await page
      .locator(".model-list")
      .getByRole("button", { name: model, exact: true })
      .click();
    await expect
      .poll(
        async () =>
          (await page
            .locator(".viewport")
            .getAttribute("data-loaded-model")) === model ||
          (await page.locator(".viewport-error").isVisible()),
        { timeout: 25000 },
      )
      .toBe(true);
    await expect(page.locator(".viewport-error")).not.toBeVisible();
    await expect(page.locator(".viewport")).toHaveAttribute(
      "data-loaded-model",
      model,
    );
    expect(
      Number(await page.locator(".viewport").getAttribute("data-meshes")),
    ).toBeGreaterThan(0);
    await expect(page.locator(".viewport-error")).not.toBeVisible();
    await expect(page.locator(".viewport canvas")).toBeVisible();
    if (model === "Goblin") {
      await page.getByLabel("Animation clip").selectOption("-1");
      await page.screenshot({ path: "build/model-rest.png" });
      await page
        .getByLabel("Animation clip")
        .selectOption({ label: "gob_vihren_walk_03" });
      await page
        .getByRole("button", { name: "Pause animation", exact: true })
        .click();
      const scrub = async (time: string) =>
        page.getByLabel("Animation time").evaluate((element, value) => {
          (element as HTMLInputElement).value = value;
          element.dispatchEvent(new Event("input", { bubbles: true }));
        }, time);
      await scrub("0.1");
      const first = await page.locator(".viewport canvas").screenshot();
      await scrub("0.6");
      const second = await page.locator(".viewport canvas").screenshot();
      expect(first.equals(second)).toBe(false);
      await page.screenshot({ path: "build/model-goblin.png" });
      await page
        .getByRole("button", { name: "Play animation", exact: true })
        .click();
    }
    if (model === "Gate01") {
      await page.getByLabel("Animation clip").selectOption({ label: "stand" });
      const stand = await page.locator(".viewport canvas").screenshot();
      await page.getByLabel("Animation clip").selectOption({ label: "fallen" });
      const fallen = await page.locator(".viewport canvas").screenshot();
      expect(stand.equals(fallen)).toBe(false);
      await page.screenshot({ path: "build/model-gate.png" });
    }
  }
  expect(errors).toEqual([]);
});
