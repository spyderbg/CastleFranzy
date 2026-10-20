<script setup lang="ts">
import { computed, ref, shallowRef, onMounted, onUnmounted, watch } from "vue";
import { frameBounds, moduleSize, type SpriteRect } from "../../shared/sprite";
import { drawModule, drawFrame, type EditorMode, type Images } from "./preview";
import type { SpriteDocument } from "../../shared/sprite";
interface CanvasProps {
  sprite: SpriteDocument;
  images: Images;
  mode: EditorMode;
  primary: number;
  secondary: number;
  imageIndex: number;
  step: number;
  zoom: number;
  grid: boolean;
  origin: boolean;
  rects: boolean;
  all: boolean;
  onion: boolean;
  draw: "module" | "rect" | null;
  viewKey?: number;
}
const p = defineProps<CanvasProps>();
const emit = defineEmits<{
  select: [index: number];
  "drag-start": [];
  move: [index: number, x: number, y: number];
  draw: [rect: SpriteRect];
  zoom: [zoom: number];
  cursor: [x: number, y: number];
}>();
const canvas = shallowRef<HTMLCanvasElement | null>(null);
const container = shallowRef<HTMLDivElement | null>(null);
const size = ref({ w: 600, h: 400 });
const pan = ref({ x: 0, y: 0 });
const draft = ref<SpriteRect | null>(null);
const drag = shallowRef<{
  x: number;
  y: number;
  index: number;
  ox: number;
  oy: number;
  started: boolean;
  pan: boolean;
  draw: boolean;
} | null>(null);
let cleanup1: (() => void) | void;
onMounted(() => {
  cleanup1 = (() => {
    const observer = new ResizeObserver((entries) => {
      const r = entries[0].contentRect;
      size.value = { w: r.width, h: r.height };
    });
    if (container.value) observer.observe(container.value);
    return () => observer.disconnect();
  })();
});
onUnmounted(() => cleanup1?.());
watch(
  [() => canvas.value, () => p.mode, () => p.imageIndex, () => p.viewKey],
  (_values, _old, onCleanup) => {
    pan.value = { x: 0, y: 0 };
  },
  { immediate: true, flush: "post" },
);
const scale = computed(() => p.zoom / 100);
const atlas = computed(() => p.mode === "modules" && p.all);
const img = computed(() => p.images[p.imageIndex]);
const origin = computed(() => ({
  x:
    size.value.w / 2 +
    pan.value.x -
    (atlas.value ? ((img.value?.width || 0) * scale.value) / 2 : 0),
  y:
    size.value.h / 2 +
    pan.value.y -
    (atlas.value ? ((img.value?.height || 0) * scale.value) / 2 : 0),
}));
const toPoint = (event: PointerEvent) => {
  const r = canvas.value!.getBoundingClientRect();
  return {
    x: Math.round((event.clientX - r.left - origin.value.x) / scale.value),
    y: Math.round((event.clientY - r.top - origin.value.y) / scale.value),
  };
};
watch(
  [
    () => canvas.value,
    () => p.sprite,
    () => p.images,
    () => p.mode,
    () => p.primary,
    () => p.secondary,
    () => p.imageIndex,
    () => p.step,
    () => p.zoom,
    () => p.grid,
    () => p.origin,
    () => p.rects,
    () => p.all,
    () => p.onion,
    () => size.value,
    () => pan.value,
    () => draft.value,
  ],
  (_values, _old, onCleanup) => {
    const c = canvas.value;
    if (!c) return;
    const ratio = window.devicePixelRatio || 1;
    c.width = Math.round(size.value.w * ratio);
    c.height = Math.round(size.value.h * ratio);
    const ctx = c.getContext("2d")!;
    ctx.scale(ratio, ratio);
    ctx.imageSmoothingEnabled = false;
    ctx.fillStyle = "#fff";
    ctx.fillRect(0, 0, size.value.w, size.value.h);
    const tile = 12;
    ctx.fillStyle = "#f2f2f2";
    for (let y = 0; y < size.value.h; y += tile)
      for (let x = 0; x < size.value.w; x += tile)
        if ((x / tile + y / tile) % 2 === 0) ctx.fillRect(x, y, tile, tile);
    ctx.translate(origin.value.x, origin.value.y);
    ctx.scale(scale.value, scale.value);
    if (p.grid && scale.value >= 0.5) {
      ctx.strokeStyle = "#d9dce3";
      ctx.lineWidth = 1 / scale.value;
      ctx.beginPath();
      const step = 16;
      for (
        let x = Math.floor(-origin.value.x / scale.value / step) * step;
        x < (size.value.w - origin.value.x) / scale.value;
        x += step
      ) {
        ctx.moveTo(x, -origin.value.y / scale.value);
        ctx.lineTo(x, (size.value.h - origin.value.y) / scale.value);
      }
      for (
        let y = Math.floor(-origin.value.y / scale.value / step) * step;
        y < (size.value.h - origin.value.y) / scale.value;
        y += step
      ) {
        ctx.moveTo(-origin.value.x / scale.value, y);
        ctx.lineTo((size.value.w - origin.value.x) / scale.value, y);
      }
      ctx.stroke();
    }
    if (p.origin) {
      ctx.strokeStyle = "#7d8da6";
      ctx.lineWidth = 1 / scale.value;
      ctx.beginPath();
      ctx.moveTo(-origin.value.x / scale.value, 0);
      ctx.lineTo((size.value.w - origin.value.x) / scale.value, 0);
      ctx.moveTo(0, -origin.value.y / scale.value);
      ctx.lineTo(0, (size.value.h - origin.value.y) / scale.value);
      ctx.stroke();
    }
    const outline = (r: SpriteRect, color: string, dashed = false) => {
      ctx.strokeStyle = color;
      ctx.lineWidth = 1.5 / scale.value;
      ctx.setLineDash(dashed ? [4 / scale.value, 3 / scale.value] : []);
      ctx.strokeRect(r.x1, r.y1, r.x2 - r.x1, r.y2 - r.y1);
      ctx.setLineDash([]);
    };
    if (p.mode === "modules") {
      if (atlas.value) {
        if (img.value) ctx.drawImage(img.value, 0, 0);
        p.sprite.modules.forEach((m, i) => {
          if (m.type === "MD_IMAGE" && m.values[0] === p.imageIndex)
            outline(
              {
                x1: m.values[1],
                y1: m.values[2],
                x2: m.values[1] + m.values[3],
                y2: m.values[2] + m.values[4],
              },
              i === p.primary ? "#e13443" : "#31916288",
            );
        });
      } else {
        const m = p.sprite.modules[p.primary];
        if (m) {
          const [w, h] = moduleSize(m);
          drawModule(ctx, m, p.images, -w / 2, -h / 2);
          outline({ x1: -w / 2, y1: -h / 2, x2: w / 2, y2: h / 2 }, "#cc3547");
        }
      }
    } else {
      let f = p.sprite.frames[p.primary];
      let af =
        p.mode === "animations"
          ? p.sprite.animations[p.primary]?.frames[p.step]
          : undefined;
      if (af) f = p.sprite.frames.find((f) => f.id === af.target)!;
      if (f) {
        if (p.onion && p.mode === "animations" && p.step > 0) {
          const prev = p.sprite.animations[p.primary].frames[p.step - 1];
          ctx.globalAlpha = 0.2;
          drawFrame(ctx, p.sprite, prev.target, p.images);
          ctx.globalAlpha = 1;
        }
        ctx.save();
        if (af) {
          ctx.translate(af.x, af.y);
          ctx.scale(af.flags & 1 ? -1 : 1, af.flags & 2 ? -1 : 1);
          if (af.flags & 4) ctx.rotate(Math.PI / 2);
        }
        drawFrame(ctx, p.sprite, f.id, p.images);
        if (p.rects) {
          try {
            outline(frameBounds(p.sprite, f), "#218649", true);
          } catch {}
          for (const r of f.rects) outline(r, "#a638b5");
        }
        if (p.mode === "frames") {
          const part = f.parts[p.secondary];
          const m =
            part &&
            !(part.flags & 16) &&
            p.sprite.modules.find((m) => m.id === part.target);
          if (m) {
            let [w, h] = moduleSize(m);
            if (part.flags & 4) [w, h] = [h, w];
            outline(
              { x1: part.x, y1: part.y, x2: part.x + w, y2: part.y + h },
              "#de3048",
            );
          }
        }
        ctx.restore();
      }
    }
    if (draft.value) outline(draft.value, "#d013ba", true);
  },
  { immediate: true, flush: "post" },
);
function down(event: PointerEvent) {
  const point = toPoint(event);
  (event.currentTarget as HTMLCanvasElement).setPointerCapture(event.pointerId);
  if (event.button === 1 || event.altKey) {
    drag.value = {
      ...point,
      index: -1,
      ox: pan.value.x,
      oy: pan.value.y,
      started: false,
      pan: true,
      draw: false,
    };
    return;
  }
  if (p.draw) {
    drag.value = {
      ...point,
      index: -1,
      ox: 0,
      oy: 0,
      started: false,
      pan: false,
      draw: true,
    };
    draft.value = { x1: point.x, y1: point.y, x2: point.x, y2: point.y };
    return;
  }
  let index = -1,
    ox = 0,
    oy = 0;
  if (atlas.value) {
    for (let i = p.sprite.modules.length - 1; i >= 0; i--) {
      const m = p.sprite.modules[i];
      if (
        m.type === "MD_IMAGE" &&
        m.values[0] === p.imageIndex &&
        point.x >= m.values[1] &&
        point.y >= m.values[2] &&
        point.x < m.values[1] + m.values[3] &&
        point.y < m.values[2] + m.values[4]
      ) {
        index = i;
        ox = m.values[1];
        oy = m.values[2];
        break;
      }
    }
  } else if (p.mode === "frames") {
    const f = p.sprite.frames[p.primary];
    if (f)
      for (let i = f.parts.length - 1; i >= 0; i--) {
        const part = f.parts[i];
        const m = p.sprite.modules.find((m) => m.id === part.target);
        if (!m) continue;
        let [w, h] = moduleSize(m);
        if (part.flags & 4) [w, h] = [h, w];
        if (
          point.x >= part.x &&
          point.x <= part.x + w &&
          point.y >= part.y &&
          point.y <= part.y + h
        ) {
          index = i;
          ox = part.x;
          oy = part.y;
          break;
        }
      }
  }
  if (index >= 0) {
    emit("select", index);
    drag.value = {
      ...point,
      index,
      ox,
      oy,
      started: false,
      pan: false,
      draw: false,
    };
  }
}
function move(event: PointerEvent) {
  const point = toPoint(event);
  emit("cursor", point.x, point.y);
  const d = drag.value;
  if (!d) return;
  if (d.pan) {
    const r = canvas.value!.getBoundingClientRect();
    pan.value = {
      x:
        event.clientX -
        r.left -
        size.value.w / 2 +
        (atlas.value ? ((img.value?.width || 0) * scale.value) / 2 : 0) -
        d.x * scale.value,
      y:
        event.clientY -
        r.top -
        size.value.h / 2 +
        (atlas.value ? ((img.value?.height || 0) * scale.value) / 2 : 0) -
        d.y * scale.value,
    };
    return;
  }
  if (d.draw) {
    draft.value = {
      x1: Math.min(d.x, point.x),
      y1: Math.min(d.y, point.y),
      x2: Math.max(d.x, point.x),
      y2: Math.max(d.y, point.y),
    };
    return;
  }
  if (point.x === d.x && point.y === d.y) return;
  if (!d.started) {
    emit("drag-start");
    d.started = true;
  }
  emit("move", d.index, d.ox + point.x - d.x, d.oy + point.y - d.y);
}
function up() {
  if (
    drag.value?.draw &&
    draft.value &&
    draft.value.x2 > draft.value.x1 &&
    draft.value.y2 > draft.value.y1
  )
    emit("draw", draft.value);
  drag.value = null;
  draft.value = null;
}
</script>
<template>
  <div :class="['canvas-container', { drawing: p.draw }]" ref="container">
    <canvas
      aria-label="Sprite preview"
      ref="canvas"
      @pointerdown="down"
      @pointermove="move"
      @pointerup="up"
      @pointercancel="up"
      @contextmenu.prevent
      @wheel.prevent="
        emit(
          'zoom',
          Math.max(
            12.5,
            Math.min(1600, p.zoom * ($event.deltaY > 0 ? 0.8 : 1.25)),
          ),
        )
      "
    />
  </div>
</template>
