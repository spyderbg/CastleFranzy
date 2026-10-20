import { shallowRef, watch, type Ref } from "vue";
import {
  frameBounds,
  moduleSize,
  type SpriteDocument,
  type SpriteModule,
  type SpriteRect,
} from "../../shared/sprite";
export type EditorMode = "modules" | "frames" | "animations" | "source";
export type Images = Record<number, HTMLImageElement>;

export function useImages(
  sprite: Readonly<Ref<SpriteDocument | null>>,
  file: () => string,
  transparency: Readonly<Ref<boolean>>,
) {
  const images = shallowRef<Images>({});
  const error = shallowRef("");
  watch(
    [() => JSON.stringify(sprite.value?.images), file, transparency],
    (_values, _old, onCleanup) => {
      let active = true;
      onCleanup(() => {
        active = false;
      });
      error.value = "";
      images.value = {};
      if (!sprite.value || !file()) return;
      const values: Images = {};
      sprite.value.images.forEach((item, index) => {
        const image = new Image();
        const query = new URLSearchParams({ sprite: file(), file: item.file });
        if (transparency.value && item.transparent !== undefined)
          query.set("transparent", String(item.transparent));
        if (item.alphaFile) query.set("alpha", item.alphaFile);
        image.onload = () => {
          if (active) {
            values[index] = image;
            images.value = { ...values };
          }
        };
        image.onerror = () => {
          if (active)
            error.value = `Cannot preview ${item.file}. Check its path and PNG/TGA/BMP format.`;
        };
        image.src = "/api/image?" + query;
      });
    },
    { immediate: true },
  );
  return { images, error };
}

export function drawModule(
  ctx: CanvasRenderingContext2D,
  m: SpriteModule,
  images: Images,
  x = 0,
  y = 0,
  flags = 0,
) {
  const [width, height] = moduleSize(m);
  const rotated = !!(flags & 4);
  const w = rotated ? height : width,
    h = rotated ? width : height;
  ctx.save();
  ctx.translate(x + w / 2, y + h / 2);
  ctx.scale(flags & 1 ? -1 : 1, flags & 2 ? -1 : 1);
  if (rotated) ctx.rotate(Math.PI / 2);
  if (m.type === "MD_IMAGE") {
    const image = images[m.values[0]];
    if (image && width > 0 && height > 0)
      ctx.drawImage(
        image,
        m.values[1],
        m.values[2],
        width,
        height,
        -width / 2,
        -height / 2,
        width,
        height,
      );
    else {
      ctx.fillStyle = "#babac0";
      ctx.fillRect(-width / 2, -height / 2, width, height);
    }
  } else {
    const color = m.values[0] >>> 0;
    ctx.fillStyle = ctx.strokeStyle =
      "#" + (color & 0xffffff).toString(16).padStart(6, "0");
    ctx.lineWidth = 1;
    if (m.type.includes("ARC")) {
      ctx.beginPath();
      ctx.ellipse(
        0,
        0,
        width / 2,
        height / 2,
        ((m.values[3] || 0) * Math.PI) / 180,
        0,
        ((m.values[4] || 360) * Math.PI) / 180,
      );
      m.type.includes("FILL") ? ctx.fill() : ctx.stroke();
    } else if (m.type.includes("TRIANGLE")) {
      ctx.beginPath();
      ctx.moveTo(-width / 2, -height / 2);
      ctx.lineTo(m.values[1], m.values[2]);
      ctx.lineTo(m.values[3], m.values[4]);
      ctx.closePath();
      m.type.includes("FILL") ? ctx.fill() : ctx.stroke();
    } else if (m.type === "MD_MARKER") {
      ctx.strokeStyle = "#c800d8";
      ctx.beginPath();
      ctx.moveTo(-6, 0);
      ctx.lineTo(6, 0);
      ctx.moveTo(0, -6);
      ctx.lineTo(0, 6);
      ctx.stroke();
    } else
      m.type.includes("FILL")
        ? ctx.fillRect(-width / 2, -height / 2, width, height)
        : ctx.strokeRect(-width / 2, -height / 2, width, height);
  }
  ctx.restore();
}
export function drawFrame(
  ctx: CanvasRenderingContext2D,
  s: SpriteDocument,
  id: number,
  images: Images,
  seen = new Set<number>(),
) {
  const frame = s.frames.find((f) => f.id === id);
  if (!frame || seen.has(id)) return;
  const next = new Set(seen).add(id);
  frame.parts.forEach((p) => {
    if (p.flags & 16) {
      ctx.save();
      ctx.translate(p.x, p.y);
      ctx.scale(p.flags & 1 ? -1 : 1, p.flags & 2 ? -1 : 1);
      if (p.flags & 4) ctx.rotate(Math.PI / 2);
      drawFrame(ctx, s, p.target, images, next);
      ctx.restore();
    } else {
      const m = s.modules.find((m) => m.id === p.target);
      if (m) drawModule(ctx, m, images, p.x, p.y, p.flags);
    }
  });
}
