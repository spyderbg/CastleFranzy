<script setup lang="ts">
import { ref } from "vue";
import type { Day, Unit, GameStatus, Wave } from "../../shared/level";
import UnitIcon from "./UnitIcon.vue";
const props = defineProps<{
  day: Day;
  selected: number;
  units: Unit[];
  assets: string;
  live?: GameStatus;
}>();
const emit = defineEmits<{ select: [index: number]; checkpoint: [] }>();
const zoom = ref(1);
let drag:
  | {
      wave: Wave;
      key: "timeStart" | "timeInterval" | "repeatInterval";
      x: number;
      value: number;
      width: number;
      pointer: number;
    }
  | undefined;
function start(e: PointerEvent, wave: Wave, index: number) {
  if (e.button > 2) return;
  e.preventDefault();
  emit("select", index);
  emit("checkpoint");
  const element = e.currentTarget as HTMLElement;
  element.setPointerCapture(e.pointerId);
  drag = {
    wave,
    key:
      e.button === 2
        ? "timeInterval"
        : e.button === 1
          ? "repeatInterval"
          : "timeStart",
    x: e.clientX,
    value:
      e.button === 2
        ? wave.timeInterval
        : e.button === 1
          ? wave.repeatInterval
          : wave.timeStart,
    width: element.clientWidth,
    pointer: e.pointerId,
  };
}
function end() {
  drag = undefined;
}
function move(e: PointerEvent) {
  if (drag)
    drag.wave[drag.key] = Math.max(
      0,
      Math.min(
        999,
        Math.round(
          drag.value +
            ((e.clientX - drag.x) / drag.width) * props.day.dayEndTime,
        ),
      ),
    );
}
function wheel(e: WheelEvent, wave: Wave) {
  e.preventDefault();
  emit("checkpoint");
  wave.repeatCount = Math.max(
    1,
    Math.min(99, wave.repeatCount + (e.deltaY < 0 ? 1 : -1)),
  );
}
function liveRows() {
  const rows: number[] = [];
  return (props.live?.units || []).map((u) => {
    let row = rows.findIndex((end) => end <= u.time);
    if (row < 0) row = rows.length;
    rows[row] = u.time + 2;
    return { ...u, row };
  });
}
</script>
<template>
  <div class="timeline-caption">
    <span>WAVE TIMELINE <small>seconds</small></span
    ><label
      >Zoom
      <input
        aria-label="Timeline zoom"
        type="range"
        min="1"
        max="5"
        step=".25"
        v-model.number="zoom"
    /></label>
  </div>
  <div class="timeline-scroll">
    <div class="timeline-content" :style="{ minWidth: `${zoom * 100}%` }">
      <div class="ruler">
        <span class="wave-label">WAVE</span>
        <div>
          <span
            v-for="tick in 11"
            :key="tick"
            :style="{ left: `${(tick - 1) * 10}%` }"
            >{{ Math.round(((tick - 1) * day.dayEndTime) / 10) }}</span
          >
        </div>
      </div>
      <div
        v-for="(wave, index) in day.waves"
        :key="index"
        class="timeline-row"
        :class="{ selected: selected === index }"
      >
        <button class="wave-label" @click="emit('select', index)">
          <UnitIcon
            :assets="assets"
            :unit="
              units.find((u) => u.name === wave.variants[0]?.enemies[0]?.type)
            "
          /><span>{{ wave.name || `Wave ${index + 1}` }}</span>
        </button>
        <div
          class="track"
          :aria-label="`Timeline wave ${index + 1}`"
          @pointerdown="start($event, wave, index)"
          @pointermove="move"
          @pointerup="end"
          @pointercancel="end"
          @contextmenu.prevent
          @wheel="wheel($event, wave)"
        >
          <span
            v-for="rep in wave.repeatCount"
            :key="rep"
            class="wave-block"
            :title="`${wave.timeStart + (rep - 1) * wave.repeatInterval}s · ${wave.timeInterval}s`"
            :style="{
              left: `${((wave.timeStart + (rep - 1) * wave.repeatInterval) / day.dayEndTime) * 100}%`,
              width: `${(wave.timeInterval / day.dayEndTime) * 100}%`,
            }"
          ></span>
        </div>
      </div>
      <p v-if="!day.waves.length" class="empty">
        No waves yet. Add a wave to begin.
      </p>
      <div v-if="live?.valid" class="live-timeline">
        <span class="wave-label"
          >LIVE · {{ Math.max(0, live.time || 0).toFixed(1) }}s</span
        >
        <div
          class="live-track"
          :style="{
            height: `${Math.max(3, ...liveRows().map((u) => u.row + 1)) * 24}px`,
          }"
        >
          <span
            class="playhead"
            :style="{
              left: `${(Math.max(0, live.time || 0) / day.dayEndTime) * 100}%`,
            }"
          ></span>
          <span
            v-for="(unit, i) in liveRows()"
            :key="i"
            class="live-unit"
            :class="unit.state"
            :title="`${units[unit.type]?.name} · ${unit.time.toFixed(1)}s · ${unit.state}${unit.extra ? ' · extra spawn' : ''}`"
            :style="{
              left: `${(unit.time / day.dayEndTime) * 100}%`,
              top: `${unit.row * 24}px`,
            }"
            ><UnitIcon :assets="assets" :unit="units[unit.type]"
          /></span>
        </div>
      </div>
    </div>
  </div>
  <p class="hint">
    Drag: start time · Right-drag: duration · Middle-drag: repeat interval ·
    Wheel: repeat count
    <span v-if="live?.valid">
      · <b class="alive">Alive</b> / <b class="next">Next</b> / Future /
      Dead</span
    >
  </p>
</template>
