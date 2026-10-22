<script setup lang="ts">
import {
  computed,
  ref,
  watch,
  onMounted,
  onBeforeUnmount,
  nextTick,
} from "vue";
import {
  clone,
  newDay,
  newWave,
  newLevel,
  challenges,
  weapons,
  spells,
  type Project,
  type OpenProject,
  type Day,
  type GameStatus,
} from "../../shared/level";
import { api, session } from "./api";
import { useBackendLifetime } from "./useBackendLifetime";
import WaveTimeline from "./WaveTimeline.vue";
import UnitIcon from "./UnitIcon.vue";
import ModelPreview from "./ModelPreview.vue";
useBackendLifetime();
const opened = ref<OpenProject>(),
  project = ref<Project>({ levels: [] }),
  saved = ref(""),
  revision = ref(""),
  assets = ref("Build/bin");
const tab = ref("Waves"),
  levelIndex = ref(0),
  dayIndex = ref(0),
  waveIndex = ref(0),
  modelIndex = ref(0),
  modelSearch = ref("");
const busy = ref(false),
  error = ref(""),
  notice = ref("Loading project…"),
  clipboard = ref<Day>(),
  game = ref<GameStatus>({ running: false }),
  showLive = ref(true),
  showLog = ref(false);
const undoStack = ref<string[]>([]),
  redoStack = ref<string[]>([]);
let ignoreHistory = false,
  poll: ReturnType<typeof setInterval>,
  polling = false;
const pauseWhileEditing = ref(true);
const dirty = computed(() => JSON.stringify(project.value) !== saved.value);
const level = computed(() => project.value.levels[levelIndex.value]),
  day = computed(() => level.value?.days[dayIndex.value]),
  wave = computed(() => day.value?.waves[waveIndex.value]);
const endless = computed(
  () => levelIndex.value === project.value.levels.length - 1,
);
const models = computed(
  () =>
    opened.value?.models.filter((m) =>
      m.name.toLowerCase().includes(modelSearch.value.toLowerCase()),
    ) || [],
);
const visibleUnits = computed(
  () => opened.value?.units.filter((u) => !u.hidden) || [],
);
const live = computed(() =>
  showLive.value &&
  game.value.running &&
  game.value.valid &&
  game.value.level === levelIndex.value &&
  game.value.day === dayIndex.value
    ? game.value
    : undefined,
);
function labelLevel(index: number) {
  return index === project.value.levels.length - 1
    ? "Endless"
    : `Level ${String(index + 1).padStart(2, "0")}`;
}
function labelDay(index: number) {
  if (!endless.value) return `Day ${String(index + 1).padStart(2, "0")}`;
  const difficulty = level.value.days[index].difficulty;
  return `${["Easy", "Medium", "Hard"][difficulty]} ${level.value.days.slice(0, index + 1).filter((d) => d.difficulty === difficulty).length}`;
}
watch(
  () => JSON.stringify(project.value),
  (next, previous) => {
    if (!ignoreHistory && previous !== next && opened.value) {
      undoStack.value.push(previous);
      if (undoStack.value.length > 100) undoStack.value.shift();
      redoStack.value = [];
    }
  },
);
watch(levelIndex, () => {
  dayIndex.value = 0;
  waveIndex.value = 0;
});
watch(dayIndex, () => {
  waveIndex.value = 0;
});
async function history(redo = false) {
  const from = redo ? redoStack.value : undoStack.value,
    to = redo ? undoStack.value : redoStack.value,
    data = from.pop();
  if (!data) return;
  to.push(JSON.stringify(project.value));
  ignoreHistory = true;
  project.value = JSON.parse(data);
  clamp();
  await nextTick();
  ignoreHistory = false;
}
function clamp() {
  levelIndex.value = Math.max(
    0,
    Math.min(levelIndex.value, project.value.levels.length - 1),
  );
  dayIndex.value = Math.max(
    0,
    Math.min(dayIndex.value, level.value?.days.length - 1 || 0),
  );
  waveIndex.value = Math.max(
    0,
    Math.min(waveIndex.value, day.value?.waves.length - 1 || 0),
  );
}
async function run(action: () => Promise<unknown>) {
  busy.value = true;
  error.value = "";
  try {
    await action();
  } catch (e) {
    error.value = (e as Error).message;
  } finally {
    busy.value = false;
  }
}
async function load(target = assets.value) {
  const result = await api<OpenProject>(
    `/api/project?assets=${encodeURIComponent(target)}`,
  );
  ignoreHistory = true;
  opened.value = result;
  project.value = result.project;
  saved.value = JSON.stringify(project.value);
  revision.value = result.revision;
  assets.value = result.assets;
  undoStack.value = [];
  redoStack.value = [];
  clamp();
  notice.value = `Loaded ${project.value.levels.length} levels from ${result.assets}/Levels`;
  await nextTick();
  ignoreHistory = false;
}
async function save() {
  const snapshot = clone(project.value);
  const result = await api("/api/save", {
    assets: assets.value,
    project: snapshot,
    revision: revision.value,
  });
  revision.value = result.revision;
  saved.value = JSON.stringify(snapshot);
  notice.value = result.changed.length
    ? `Saved ${result.changed.length} files · backup: ${result.backup}`
    : "Saved · no files changed";
}
function reload() {
  if (!dirty.value || window.confirm("Discard unsaved changes and reload?"))
    void run(() => load());
}
function switchAssets(e: Event) {
  const target = (e.target as HTMLSelectElement).value;
  (e.target as HTMLSelectElement).value = assets.value;
  if (
    !dirty.value ||
    window.confirm("Discard unsaved changes and switch assets?")
  )
    void run(() => load(target));
}
function addLevel() {
  const at = Math.max(0, project.value.levels.length - 1);
  project.value.levels.splice(at, 0, newLevel());
  levelIndex.value = at;
}
function deleteLevel() {
  if (!level.value) return;
  project.value.levels.splice(levelIndex.value, 1);
  clamp();
}
function moveLevel(offset: number) {
  const at = levelIndex.value,
    to = at + offset;
  if (to < 0 || to >= project.value.levels.length) return;
  const [item] = project.value.levels.splice(at, 1);
  project.value.levels.splice(to, 0, item);
  levelIndex.value = to;
}
function addDay(d = newDay()) {
  level.value.days.push(d);
  dayIndex.value = level.value.days.length - 1;
}
function cutDay() {
  clipboard.value = clone(day.value);
  level.value.days.splice(dayIndex.value, 1);
  clamp();
}
function moveDay(offset: number) {
  const at = dayIndex.value,
    to = at + offset;
  if (to < 0 || to >= level.value.days.length) return;
  const [item] = level.value.days.splice(at, 1);
  level.value.days.splice(to, 0, item);
  dayIndex.value = to;
}
function addWave() {
  day.value.waves.push(newWave(day.value.waves.length));
  waveIndex.value = day.value.waves.length - 1;
}
function deleteWave() {
  day.value.waves.splice(waveIndex.value, 1);
  clamp();
}
function cloneWave() {
  day.value.waves.push(clone(wave.value));
  waveIndex.value = day.value.waves.length - 1;
}
function moveWave(offset: number) {
  const at = waveIndex.value,
    to = at + offset;
  if (to < 0 || to >= day.value.waves.length) return;
  const [item] = day.value.waves.splice(at, 1);
  day.value.waves.splice(to, 0, item);
  waveIndex.value = to;
}
async function gameCommand(command: string) {
  if (command === "play") await save();
  game.value = await api("/api/game", {
    command,
    assets: assets.value,
    level: levelIndex.value,
    day: tab.value === "Weapons" ? 0 : dayIndex.value,
  });
  notice.value =
    command === "play"
      ? "Test game started. Use E/R in the game for its built-in editor."
      : `Game: ${command}`;
}
async function exit() {
  await save();
  await api("/api/exit", {});
  notice.value = "Saved. Level Editor closed.";
  window.close();
}
function key(e: KeyboardEvent) {
  const editing = (e.target as HTMLElement)?.closest(
    "input,select,textarea,[contenteditable]",
  );
  if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === "s") {
    e.preventDefault();
    if (!busy.value) void run(save);
  } else if (
    !editing &&
    (e.ctrlKey || e.metaKey) &&
    ["z", "y"].includes(e.key.toLowerCase())
  ) {
    e.preventDefault();
    void history(e.key.toLowerCase() === "y" || e.shiftKey);
  } else if (!editing && e.key === "Escape" && !busy.value) void run(exit);
}
function unload(e: BeforeUnloadEvent) {
  if (dirty.value) {
    e.preventDefault();
    e.returnValue = "";
  }
}
function focusEditor() {
  if (
    pauseWhileEditing.value &&
    game.value.running &&
    !game.value.paused &&
    !busy.value
  )
    void run(() => gameCommand("pause"));
}
onMounted(async () => {
  window.addEventListener("keydown", key);
  window.addEventListener("beforeunload", unload);
  window.addEventListener("focus", focusEditor);
  await run(async () => {
    const s = await session();
    assets.value = s.assets;
    await load();
  });
  poll = setInterval(async () => {
    if (polling) return;
    polling = true;
    try {
      game.value = await api("/api/game");
    } catch {
    } finally {
      polling = false;
    }
  }, 600);
});
onBeforeUnmount(() => {
  clearInterval(poll);
  window.removeEventListener("keydown", key);
  window.removeEventListener("beforeunload", unload);
  window.removeEventListener("focus", focusEditor);
});
</script>
<template>
  <div class="app-shell">
    <header class="app-header">
      <div class="brand-mark">♜</div>
      <div><strong>CASTLE FRENZY</strong><span>Level Editor</span></div>
      <span class="version">WORKSPACE / 01</span>
      <div class="header-actions">
        <span class="dirty-dot" :class="{ dirty }">{{
          dirty ? "Unsaved changes" : "All changes saved"
        }}</span
        ><button :disabled="busy || !opened" @click="run(save)">
          Save <kbd>Ctrl S</kbd></button
        ><button :disabled="busy || !opened" @click="run(exit)">
          Save &amp; exit
        </button>
      </div>
    </header>
    <div class="workspace-bar">
      <label
        >Asset directory
        <select
          aria-label="Asset directory"
          :value="assets"
          :disabled="busy || game.running"
          @change="switchAssets"
        >
          <option v-for="root in opened?.assetRoots" :key="root">
            {{ root }}
          </option>
        </select></label
      >
      <div class="tools">
        <button :disabled="!undoStack.length || busy" @click="history()">
          Undo</button
        ><button :disabled="!redoStack.length || busy" @click="history(true)">
          Redo</button
        ><button :disabled="busy" @click="reload">Reload</button>
      </div>
    </div>
    <nav class="tabs" aria-label="Editor tabs">
      <button
        v-for="(name, i) in ['Waves', 'Weapons', 'Models']"
        :key="name"
        :class="{ active: tab === name }"
        @click="tab = name"
      >
        <span>0{{ i + 1 }}</span
        >{{ name }}</button
      ><span class="tab-meta"
        >{{ project.levels.reduce((sum, l) => sum + l.days.length, 0) }} days ·
        {{ opened?.units.length || 0 }} unit types</span
      >
    </nav>
    <div v-if="error" class="error-banner" role="alert">
      {{ error }}<button @click="error = ''">Dismiss</button>
    </div>
    <main v-if="opened" :aria-busy="busy">
      <aside v-if="tab !== 'Models'" class="sidebar">
        <div class="sidebar-heading">
          LEVELS <span>{{ project.levels.length }}</span>
        </div>
        <div class="level-list">
          <button
            v-for="(l, i) in project.levels"
            :key="i"
            :class="{ active: i === levelIndex }"
            @click="levelIndex = i"
          >
            <span>{{ labelLevel(i) }}</span
            ><small>{{ l.days.length }} days</small>
          </button>
        </div>
        <div class="list-actions">
          <button aria-label="New level" @click="addLevel">+ New</button
          ><button
            aria-label="Delete level"
            :disabled="!level"
            @click="deleteLevel"
          >
            Delete</button
          ><button
            aria-label="Move level up"
            :disabled="levelIndex === 0"
            @click="moveLevel(-1)"
          >
            ↑</button
          ><button
            aria-label="Move level down"
            :disabled="levelIndex >= project.levels.length - 1"
            @click="moveLevel(1)"
          >
            ↓
          </button>
        </div>
        <template v-if="tab === 'Waves' && level"
          ><div class="sidebar-heading days-heading">
            {{ endless ? "SEGMENTS" : "DAYS" }}
            <span>{{ level.days.length }}</span>
          </div>
          <div class="day-list">
            <button
              v-for="(d, i) in level.days"
              :key="i"
              :class="{ active: dayIndex === i }"
              @click="dayIndex = i"
            >
              <span>{{ labelDay(i) }}</span
              ><small>{{ d.waves.length }} waves</small>
            </button>
          </div>
          <div class="list-actions">
            <button aria-label="New day" @click="addDay()">+ New</button
            ><button :disabled="!day" @click="cutDay">Cut</button
            ><button
              aria-label="Move day up"
              :disabled="!day || dayIndex === 0"
              @click="moveDay(-1)"
            >
              ↑</button
            ><button
              aria-label="Move day down"
              :disabled="!day || dayIndex >= level.days.length - 1"
              @click="moveDay(1)"
            >
              ↓</button
            ><button :disabled="!day" @click="clipboard = clone(day)">
              Copy</button
            ><button :disabled="!clipboard" @click="addDay(clone(clipboard!))">
              Paste
            </button>
          </div></template
        >
      </aside>
      <section v-if="tab === 'Waves'" class="workspace waves-workspace">
        <template v-if="day"
          ><div class="section-heading">
            <div>
              <div class="eyebrow">
                {{ labelLevel(levelIndex) }} / {{ labelDay(dayIndex) }}
              </div>
              <h1>Wave composition</h1>
            </div>
            <span class="count-chip">{{ day.waves.length }} waves</span>
          </div>
          <div class="day-settings">
            <label
              >Day end time
              <div class="input-unit">
                <input
                  aria-label="Day end time"
                  type="number"
                  min="1"
                  max="999"
                  v-model.number="day.dayEndTime"
                /><span>sec</span>
              </div></label
            ><label v-if="!endless"
              >Challenge
              <select aria-label="Challenge" v-model="day.challenge">
                <option v-for="challenge in challenges" :key="challenge">
                  {{ challenge }}
                </option>
              </select></label
            ><label v-else
              >Segment difficulty
              <select aria-label="Difficulty" v-model.number="day.difficulty">
                <option :value="0">Easy</option>
                <option :value="1">Medium</option>
                <option :value="2">Hard</option>
              </select></label
            ><label class="checkbox-label"
              ><input type="checkbox" v-model="showLive" /> Current day / live
              spawns</label
            >
          </div>
          <WaveTimeline
            :day="day"
            :selected="waveIndex"
            :units="opened.units"
            :assets="assets"
            :live="live"
            @select="waveIndex = $event"
          />
          <div class="wave-actions">
            <button class="primary" @click="addWave">+ New wave</button
            ><button :disabled="!wave" @click="cloneWave">Clone</button
            ><button :disabled="!wave" @click="deleteWave">Delete</button
            ><button
              aria-label="Move wave up"
              :disabled="!wave || waveIndex === 0"
              @click="moveWave(-1)"
            >
              ↑ Up</button
            ><button
              aria-label="Move wave down"
              :disabled="!wave || waveIndex >= day.waves.length - 1"
              @click="moveWave(1)"
            >
              ↓ Down
            </button>
          </div>
          <div v-if="wave" class="wave-inspector">
            <div class="inspector-heading">
              <h2>Wave {{ String(waveIndex + 1).padStart(2, "0") }}</h2>
              <label class="checkbox-label"
                ><input type="checkbox" v-model="wave.packed" /> Packed
                spawn</label
              >
            </div>
            <div class="wave-fields">
              <label
                >Name
                <input
                  aria-label="Wave name"
                  maxlength="15"
                  v-model="wave.name"
                  placeholder="Unnamed wave" /></label
              ><label
                >Start
                <input
                  aria-label="Wave start"
                  type="number"
                  min="0"
                  max="999"
                  v-model.number="wave.timeStart" /></label
              ><label
                >Duration
                <input
                  aria-label="Wave duration"
                  type="number"
                  min="0"
                  max="999"
                  v-model.number="wave.timeInterval" /></label
              ><label
                >Repeat interval
                <input
                  aria-label="Repeat interval"
                  type="number"
                  min="0"
                  max="999"
                  v-model.number="wave.repeatInterval" /></label
              ><label
                >Repeat count
                <input
                  aria-label="Repeat count"
                  type="number"
                  min="1"
                  max="99"
                  v-model.number="wave.repeatCount"
              /></label>
            </div>
            <div
              v-for="(variant, vi) in wave.variants"
              :key="vi"
              class="variant"
            >
              <div class="variant-heading">
                <h3>Variant {{ vi + 1 }}</h3>
                <label class="checkbox-label"
                  ><input type="checkbox" v-model="variant.intro" /> Intro
                  camera</label
                ><button
                  :aria-label="`Delete variant ${vi + 1}`"
                  @click="wave.variants.splice(vi, 1)"
                >
                  Remove variant
                </button>
              </div>
              <div
                v-for="(enemy, ei) in variant.enemies"
                :key="ei"
                class="enemy-row"
              >
                <UnitIcon
                  :unit="opened.units.find((u) => u.name === enemy.type)"
                  :assets="assets"
                /><label
                  >Unit type
                  <select
                    :aria-label="`Unit type ${vi + 1}-${ei + 1}`"
                    v-model="enemy.type"
                  >
                    <option
                      v-for="unit in visibleUnits"
                      :key="unit.name"
                      :value="unit.name"
                    >
                      {{ unit.name }}
                    </option>
                    <option
                      v-if="!visibleUnits.some((u) => u.name === enemy.type)"
                      :value="enemy.type"
                    >
                      {{ enemy.type }} (hidden)
                    </option>
                  </select></label
                ><label
                  >Min count
                  <input
                    :aria-label="`Min count ${vi + 1}-${ei + 1}`"
                    type="number"
                    min="1"
                    max="99"
                    v-model.number="enemy.min"
                    @change="
                      enemy.max = Math.max(enemy.min, enemy.max)
                    " /></label
                ><span class="range-dash">—</span
                ><label
                  >Max count
                  <input
                    :aria-label="`Max count ${vi + 1}-${ei + 1}`"
                    type="number"
                    min="1"
                    max="99"
                    v-model.number="enemy.max"
                    @change="
                      enemy.min = Math.min(enemy.min, enemy.max)
                    " /></label
                ><button
                  :aria-label="`Delete enemy ${vi + 1}-${ei + 1}`"
                  @click="variant.enemies.splice(ei, 1)"
                >
                  Remove
                </button>
              </div>
              <button
                class="text-button"
                @click="
                  variant.enemies.push({
                    type: visibleUnits[0]?.name || opened.units[0].name,
                    min: 3,
                    max: 5,
                  })
                "
              >
                + New enemy type
              </button>
            </div>
            <button
              class="text-button"
              @click="wave.variants.push({ intro: false, enemies: [] })"
            >
              + New variant
            </button>
          </div>
        </template>
        <div v-else class="empty">
          <h2>
            {{ level ? "Add a day to this level" : "Create a level to begin" }}
          </h2>
          <p>Each day has its own waves, challenge and duration.</p>
        </div>
      </section>
      <section v-if="tab === 'Weapons'" class="workspace">
        <template v-if="level"
          ><div class="section-heading">
            <div>
              <div class="eyebrow">{{ labelLevel(levelIndex) }}</div>
              <h1>Weapons &amp; spells</h1>
            </div>
          </div>
          <p class="description">
            Choose the starting upgrades and what the player can buy in the
            shop.
          </p>
          <table class="weapons-table">
            <thead>
              <tr>
                <th>Weapon</th>
                <th>Starting level</th>
                <th>Available in shop</th>
              </tr>
            </thead>
            <tbody>
              <tr v-for="(weapon, i) in level.loadout.weapons" :key="i">
                <th>{{ weapons[i] }}</th>
                <td>
                  <div class="segmented">
                    <button
                      v-for="n in 4"
                      :key="n"
                      :aria-label="`${weapons[i]} level ${n - 1}`"
                      :class="{ selected: weapon.startingLevel === n - 2 }"
                      @click="weapon.startingLevel = n - 2"
                    >
                      {{ n - 1 }}
                    </button>
                  </div>
                </td>
                <td>
                  <label class="checkbox-label"
                    ><input
                      :aria-label="`${weapons[i]} shop`"
                      type="checkbox"
                      v-model="weapon.shop"
                    />
                    Shop</label
                  >
                </td>
              </tr>
            </tbody>
          </table>
          <div class="spell-panel">
            <h2>Spell</h2>
            <label class="checkbox-label"
              ><input
                aria-label="Spell bought"
                type="checkbox"
                v-model="level.loadout.spellBought"
              />
              Already bought</label
            >
            <div class="segmented">
              <button
                v-for="(spell, i) in spells"
                :key="spell"
                :aria-label="`Spell ${spell}`"
                :class="{ selected: level.loadout.spell === i - 1 }"
                @click="level.loadout.spell = i - 1"
              >
                {{ spell }}
              </button>
            </div>
          </div>
        </template>
        <div v-else class="empty">Create a level to edit its weapons.</div>
      </section>
      <template v-if="tab === 'Models'"
        ><aside class="sidebar model-sidebar">
          <div class="sidebar-heading">
            MODELS <span>{{ opened.models.length }}</span>
          </div>
          <input
            class="model-search"
            aria-label="Search models"
            placeholder="Search models…"
            v-model="modelSearch"
          />
          <div class="model-list">
            <button
              v-for="model in models"
              :key="model.name"
              :class="{
                active: opened.models[modelIndex]?.name === model.name,
              }"
              @click="modelIndex = opened.models.indexOf(model)"
            >
              {{ model.name }}
            </button>
          </div>
        </aside>
        <section class="workspace model-workspace">
          <ModelPreview
            v-if="opened.models[modelIndex]"
            :model="opened.models[modelIndex]"
            :assets="assets"
          /></section
      ></template>
    </main>
    <div v-else class="empty">
      {{
        busy
          ? "Loading level data…"
          : "Could not load project. Check the error above."
      }}
    </div>
    <div class="game-bar">
      <span class="game-indicator" :class="{ connected: game.running }"></span
      ><span
        >{{
          game.running
            ? game.paused
              ? "Test game paused"
              : "Test game running"
            : "Test game disconnected"
        }}<small v-if="game.valid">
          · Level {{ (game.level || 0) + 1 }}, day {{ (game.day || 0) + 1 }} ·
          {{
            (game.time ?? 0) < -1000
              ? "Intro"
              : Math.max(0, game.time || 0).toFixed(1) + "s"
          }}</small
        ></span
      ><label class="checkbox-label"
        ><input type="checkbox" v-model="pauseWhileEditing" /> Pause while
        editing</label
      ><button
        :disabled="busy || !level?.days.length"
        class="play-button"
        @click="run(() => gameCommand('play'))"
      >
        ▶ Play {{ tab === "Weapons" ? "level" : "day" }}</button
      ><button
        :disabled="busy || !game.running"
        @click="run(() => gameCommand(game.paused ? 'resume' : 'pause'))"
      >
        {{ game.paused ? "Resume game" : "Pause game" }}</button
      ><button
        :disabled="busy || !game.valid"
        @click="run(() => gameCommand('die'))"
      >
        DIE!</button
      ><button
        :disabled="busy || !game.running"
        @click="run(() => gameCommand('overlay'))"
      >
        F9 overlay</button
      ><button
        :disabled="busy || !game.running"
        @click="run(() => gameCommand('stop'))"
      >
        Stop</button
      ><button @click="showLog = !showLog">Game log</button>
    </div>
    <pre v-if="showLog" class="game-log">{{
      game.log || "No game output yet."
    }}</pre>
    <footer>
      <span>{{ busy ? "Working…" : notice }}</span
      ><span>{{ assets }}/Levels</span>
    </footer>
  </div>
</template>
