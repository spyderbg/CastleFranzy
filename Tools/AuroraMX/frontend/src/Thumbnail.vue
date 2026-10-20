<script setup lang="ts">
import { shallowRef, watchEffect } from "vue";
import {
  frameBounds,
  moduleSize,
  type SpriteDocument,
  type SpriteRect,
} from "../../shared/sprite";
import { drawModule, drawFrame, type Images } from "./preview";
const p = defineProps<{
  sprite: SpriteDocument;
  images: Images;
  kind: "module" | "frame";
  index: number;
  selected?: boolean;
}>();
const emit = defineEmits<{ click: []; dblclick: [] }>();
const canvas = shallowRef<HTMLCanvasElement | null>(null);
watchEffect(
  () => {
    const c = canvas.value;
    if (!c) return;
    const ctx = c.getContext("2d")!;
    ctx.setTransform(1, 0, 0, 1, 0, 0);
    ctx.clearRect(0, 0, c.width, c.height);
    ctx.imageSmoothingEnabled = false;
    let rect: SpriteRect;
    try {
      rect =
        p.kind === "module"
          ? {
              x1: 0,
              y1: 0,
              x2: moduleSize(p.sprite.modules[p.index])[0],
              y2: moduleSize(p.sprite.modules[p.index])[1],
            }
          : frameBounds(p.sprite, p.sprite.frames[p.index]);
    } catch {
      return;
    }
    const w = rect.x2 - rect.x1,
      h = rect.y2 - rect.y1,
      zoom = Math.min(2, 48 / Math.max(w, h, 1));
    ctx.translate(
      30 - (w * zoom) / 2 - rect.x1 * zoom,
      27 - (h * zoom) / 2 - rect.y1 * zoom,
    );
    ctx.scale(zoom, zoom);
    if (p.kind === "module")
      drawModule(ctx, p.sprite.modules[p.index], p.images);
    else drawFrame(ctx, p.sprite, p.sprite.frames[p.index].id, p.images);
  },
  { flush: "post" },
);
</script>
<template>
  <button
    :class="['thumb', { selected: p.selected }]"
    :title="
      (p.kind === 'module'
        ? p.sprite.modules[p.index].name
        : p.sprite.frames[p.index].name) || `${p.kind} ${p.index}`
    "
    @click="emit('click')"
    @dblclick="emit('dblclick')"
  >
    <canvas width="60" height="54" ref="canvas" /><span>{{
      p.index.toString().padStart(3, "0")
    }}</span>
  </button>
</template>
