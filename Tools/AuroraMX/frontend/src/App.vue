<script setup lang="ts">
type Cell = string | number;
import { computed, ref, shallowRef, onMounted, onUnmounted, watch } from "vue";
import defaults from "../../config/defaults.json";
import { useBackendLifetime } from "./useBackendLifetime";
import {
  FilePlus,
  FolderOpen,
  Save,
  Download,
  Undo2,
  Redo2,
  Copy,
  Trash2,
  Plus,
  ArrowUp,
  ArrowDown,
  Play,
  Pause,
  SkipBack,
  SkipForward,
  Grid2X2,
  Crosshair,
  BoxSelect,
  Hammer,
  FileCode2,
  Info,
  X,
  Folder,
  ChevronRight,
  ZoomIn,
  ZoomOut,
  MousePointer2,
  SquareDashed,
} from "@lucide/vue";
import {
  emptySprite,
  hex,
  nextId,
  moduleSize,
  frameBounds,
  validateSprite,
  type OpenSprite,
  type SpriteDocument,
  type SpriteRect,
} from "../../shared/sprite";
import { serializeSprite } from "../../backend/format";
import PreviewCanvas from "./PreviewCanvas.vue";
import Thumbnail from "./Thumbnail.vue";
import { useImages, type EditorMode } from "./preview";
import Num from "./components/NumberField.vue";
import TextField from "./components/TextField.vue";
import Tool from "./components/ToolButton.vue";
import Actions from "./components/ListActions.vue";
import Table from "./components/EditorTable.vue";
type Modal =
  | "open"
  | "new"
  | "saveAs"
  | "export"
  | "batch"
  | "build"
  | "help"
  | "stats"
  | null;
const modeLabels: Record<EditorMode, string> = {
  modules: "Module Editor",
  frames: "Frame Editor",
  animations: "Animation Editor",
  source: "Source Editor",
};
useBackendLifetime();
const opened = shallowRef<OpenSprite | null>(null);
const token = ref("");
const workspace = ref("");
const mode = ref<EditorMode>("modules");
const primary = ref(0);
const secondary = ref(0);
const bank = ref(0);
const imageIndex = ref(0);
const modal = ref<Modal>(null);
const menu = ref<string | null>(null);
const dialogPath = ref("");
const outputPath = ref(defaults.batchOutput);
const directory = ref(defaults.spriteDirectory);
const entries = ref<{ name: string; path: string; directory: boolean }[]>([]);
const error = ref("");
const message = ref("Ready");
const busy = ref(false);
const filter = ref("");
const zoom = ref(100);
const viewKey = ref(0);
const grid = ref(false);
const origin = ref(true);
const rects = ref(true);
const all = ref(true);
const transparency = ref(true);
const onion = ref(false);
const draw = ref<"module" | "rect" | null>(null);
const cursor = ref({ x: 0, y: 0 });
const playing = ref(false);
const step = ref(0);
const fps = ref(defaults.animationFps);
const loop = ref(true);
const source = ref("");
const sourceBaseline = ref("");
const job = ref<{
  running: boolean;
  exitCode: number | null;
  log: string;
} | null>(null);
const batchResult = ref("");
const undo = ref<string[]>([]);
const redo = ref<string[]>([]);
const saved = shallowRef("");
const appliedSource = shallowRef<{ text: string; document: string } | null>(
  null,
);
const doc = computed(() => opened.value?.document || null);
const { images, error: imageError } = useImages(
  doc,
  () => opened.value?.path || "",
  transparency,
);
const sourceDirty = computed(() => source.value !== sourceBaseline.value);
const dirty = computed(
  () =>
    !!doc.value &&
    (JSON.stringify(doc.value) !== saved.value ||
      sourceDirty.value ||
      (appliedSource.value?.document === JSON.stringify(doc.value) &&
        appliedSource.value.text !== opened.value?.source)),
);
const publish = (value: OpenSprite | null) => {
  opened.value = value;
};
const notify = (text: string) => {
  message.value = text;
  error.value = "";
};
async function api<T = any>(route: string, body?: unknown): Promise<T> {
  const response = await fetch(
    "/api/" + route,
    body === undefined
      ? {}
      : {
          method: "POST",
          headers: {
            "Content-Type": "application/json",
            "x-auroramx-token": token.value,
          },
          body: JSON.stringify(body),
        },
  );
  if (!response.ok) {
    const data = await response.json();
    throw new Error(data.error || "Operation failed");
  }
  return response.json();
}
async function run(action: () => Promise<unknown>) {
  error.value = "";
  busy.value = true;
  try {
    await action();
  } catch (e) {
    error.value = (e as Error).message;
  } finally {
    busy.value = false;
  }
}
function remember() {
  if (!opened.value) return;
  undo.value.push(JSON.stringify(opened.value.document));
  if (undo.value.length > 100) undo.value.shift();
  redo.value = [];
}
function change(edit: (s: SpriteDocument) => void, history = true) {
  const current = opened.value;
  if (!current) return;
  if (history) remember();
  const next = structuredClone(current.document);
  edit(next);
  publish({ ...current, document: next });
}
function restore(direction: "undo" | "redo") {
  const from = direction === "undo" ? undo.value : redo.value,
    to = direction === "undo" ? redo.value : undo.value;
  const current = opened.value;
  if (!from.length || !current) return;
  to.push(JSON.stringify(current.document));
  const next = JSON.parse(from.pop()!);
  const text =
    JSON.stringify(next) === saved.value
      ? current.source
      : serializeSprite(next);
  appliedSource.value = { text, document: JSON.stringify(next) };
  publish({ ...current, document: next });
  source.value = text;
  sourceBaseline.value = text;
  playing.value = false;
}
function resetSelection() {
  primary.value = 0;
  secondary.value = 0;
  bank.value = 0;
  imageIndex.value = 0;
  step.value = 0;
  playing.value = false;
  draw.value = null;
  filter.value = "";
}
function install(value: OpenSprite) {
  appliedSource.value = {
    text: value.source,
    document: JSON.stringify(value.document),
  };
  publish(value);
  saved.value = JSON.stringify(value.document);
  undo.value = [];
  redo.value = [];
  resetSelection();
  source.value = value.source;
  sourceBaseline.value = value.source;
}
async function openFile(file: string) {
  if (dirty.value && !window.confirm("Discard unsaved changes to this sprite?"))
    return;
  install(await api<OpenSprite>("sprite?path=" + encodeURIComponent(file)));
  modal.value = null;
  notify("Opened " + file);
}
onMounted(() => {
  fetch("/api/session")
    .then((r) => r.json())
    .then(async (session) => {
      token.value = session.token;
      workspace.value = session.root;
      const file =
        new URLSearchParams(location.search).get("file") ||
        defaults.initialSprite;
      const response = await fetch(
        "/api/sprite?path=" + encodeURIComponent(file),
      );
      if (response.ok) install(await response.json());
    })
    .catch((e) => (error.value = e.message));
});
watch(
  [() => modal.value, () => directory.value],
  (_values, _old, onCleanup) => {
    if (modal.value === "open")
      api("files?directory=" + encodeURIComponent(directory.value))
        .then((value: typeof entries.value) => (entries.value = value))
        .catch((e) => (error.value = e.message));
  },
  { immediate: true, flush: "post" },
);
watch(
  [() => dirty.value],
  (_values, _old, onCleanup) => {
    const handler = (event: BeforeUnloadEvent) => {
      if (dirty.value) {
        event.preventDefault();
        event.returnValue = "";
      }
    };
    window.addEventListener("beforeunload", handler);
    onCleanup(() => window.removeEventListener("beforeunload", handler));
  },
  { immediate: true, flush: "post" },
);
watch(
  [() => modal.value],
  (_values, _old, onCleanup) => {
    if (modal.value !== "build") return;
    const timer = setInterval(
      () =>
        api("build")
          .then((value: typeof job.value) => (job.value = value))
          .catch((e) => (error.value = e.message)),
      800,
    );
    api("build").then((value: typeof job.value) => (job.value = value));
    onCleanup(() => clearInterval(timer));
  },
  { immediate: true, flush: "post" },
);
watch(
  [
    () => playing.value,
    () => step.value,
    () => primary.value,
    () => doc.value,
    () => mode.value,
    () => fps.value,
    () => loop.value,
  ],
  (_values, _old, onCleanup) => {
    if (!playing.value || !doc.value || mode.value !== "animations") return;
    const animation = doc.value.animations[primary.value];
    const frame = animation?.frames[step.value];
    if (!frame || !frame.time) {
      playing.value = false;
      return;
    }
    const timer = setTimeout(
      () => {
        if (step.value + 1 >= animation.frames.length && !loop.value) {
          playing.value = false;
          return;
        }
        step.value = (step.value + 1) % animation.frames.length;
      },
      (frame.time * 1000) / fps.value,
    );
    onCleanup(() => clearTimeout(timer));
  },
  { immediate: true, flush: "post" },
);
async function applySource() {
  if (!sourceDirty.value || !doc.value) return doc.value!;
  const result = await api<{
    document: SpriteDocument;
  }>("parse", { source: source.value });
  remember();
  appliedSource.value = {
    text: source.value,
    document: JSON.stringify(result.document),
  };
  publish({ ...opened.value!, document: result.document });
  sourceBaseline.value = source.value;
  return result.document;
}
async function switchMode(next: EditorMode) {
  await applySource();
  mode.value = next;
  resetSelection();
  if (next === "source") {
    const current = opened.value!;
    const text =
      appliedSource.value?.document === JSON.stringify(current.document)
        ? appliedSource.value.text
        : serializeSprite(current.document);
    appliedSource.value = {
      text,
      document: JSON.stringify(current.document),
    };
    source.value = text;
    sourceBaseline.value = text;
  }
}
async function save(saveAs = false) {
  if (!opened.value) return;
  const document = await applySource();
  if (saveAs || !opened.value.revision) {
    dialogPath.value = opened.value.path;
    modal.value = "saveAs";
    return;
  }
  const value = await api<OpenSprite>("save", {
    path: opened.value.path,
    document,
    revision: opened.value.revision,
    rawSource:
      appliedSource.value?.document === JSON.stringify(document)
        ? appliedSource.value.text
        : undefined,
  });
  publish(value);
  appliedSource.value = {
    text: value.source,
    document: JSON.stringify(value.document),
  };
  saved.value = JSON.stringify(value.document);
  source.value = value.source;
  sourceBaseline.value = value.source;
  notify("Saved " + value.path);
}
function newSprite() {
  if (dirty.value && !confirm("Discard unsaved changes?")) return;
  dialogPath.value = "Data/Sprites/New.sprite";
  modal.value = "new";
}
const primaryKind = computed(() =>
  mode.value === "frames"
    ? "frames"
    : mode.value === "animations"
      ? "animations"
      : "modules",
);
const primaryItems = computed(() => doc.value?.[primaryKind.value] || []);
const selected = computed(() => primaryItems.value[primary.value]);
const frame = computed(() =>
  mode.value === "frames" ? doc.value?.frames[primary.value] : null,
);
const animation = computed(() =>
  mode.value === "animations" ? doc.value?.animations[primary.value] : null,
);
const primaryTitle = computed(() =>
  mode.value === "frames"
    ? "Frames"
    : mode.value === "animations"
      ? "Animations"
      : "Modules",
);
const secondaryTitle = computed(() =>
  mode.value === "frames"
    ? "FModules"
    : mode.value === "animations"
      ? "AFrames"
      : "Images",
);
const secondaryItems = computed(() =>
  mode.value === "frames"
    ? frame.value?.parts || []
    : mode.value === "animations"
      ? animation.value?.frames || []
      : doc.value?.images || [],
);
const part = computed(() => frame.value?.parts[secondary.value]);
const aframe = computed(() => animation.value?.frames[secondary.value]);
const image = computed(() => doc.value?.images[secondary.value]);
function addPrimary(clone = false) {
  if (!doc.value || mode.value === "source") return;
  let result = primaryItems.value.length;
  change((s) => {
    const items = s[primaryKind.value];
    const id = nextId(
      items,
      mode.value === "frames"
        ? 0x2000
        : mode.value === "animations"
          ? 0x3000
          : 0x1000,
    );
    if (mode.value === "modules")
      s.modules.push(
        clone && selected.value
          ? {
              ...structuredClone(s.modules[primary.value]),
              id,
              name: s.modules[primary.value].name + " copy",
            }
          : {
              id,
              type: "MD_IMAGE",
              values: [s.images.length ? imageIndex.value : -1, 0, 0, 32, 32],
              name: "",
            },
      );
    else if (mode.value === "frames")
      s.frames.push(
        clone && selected.value
          ? {
              ...structuredClone(s.frames[primary.value]),
              id,
              name: s.frames[primary.value].name + " copy",
            }
          : { id, name: "", parts: [], rects: [] },
      );
    else
      s.animations.push(
        clone && selected.value
          ? {
              ...structuredClone(s.animations[primary.value]),
              id,
              name: s.animations[primary.value].name + " copy",
            }
          : { id, name: "", frames: [] },
      );
  });
  primary.value = result;
  secondary.value = 0;
  step.value = 0;
}
function deletePrimary() {
  if (
    !selected.value ||
    !doc.value ||
    !confirm(
      `Delete ${primaryTitle.value.slice(0, -1).toLowerCase()} ${hex(selected.value.id)} and placements that reference it? This can be undone.`,
    )
  )
    return;
  change((s) => {
    if (mode.value === "modules") {
      s.modules.splice(primary.value, 1);
      s.frames.forEach(
        (f) =>
          (f.parts = f.parts.filter(
            (p) => p.flags & 16 || p.target !== selected.value.id,
          )),
      );
    } else if (mode.value === "frames") {
      s.frames.splice(primary.value, 1);
      s.frames.forEach(
        (f) =>
          (f.parts = f.parts.filter(
            (p) => !(p.flags & 16) || p.target !== selected.value.id,
          )),
      );
      s.animations.forEach(
        (a) =>
          (a.frames = a.frames.filter((p) => p.target !== selected.value.id)),
      );
    } else s.animations.splice(primary.value, 1);
  });
  primary.value = Math.max(0, primary.value - 1);
  secondary.value = 0;
  step.value = 0;
}
function movePrimary(delta: number) {
  const target = primary.value + delta;
  if (!doc.value || target < 0 || target >= primaryItems.value.length) return;
  change((s) => {
    const list = s[primaryKind.value] as {
      id: number;
      name: string;
    }[];
    const [item] = list.splice(primary.value, 1);
    list.splice(target, 0, item);
  });
  primary.value = target;
}
function addSecondary(clone = false) {
  if (!doc.value) return;
  if (mode.value === "modules") {
    change((s) => {
      const old = s.images[secondary.value];
      s.images.push(
        clone && old
          ? { ...old, id: nextId(s.images, 0) }
          : {
              id: nextId(s.images, 0),
              file: ".\\image.png",
              transparent: 0xff00ff,
            },
      );
    });
    secondary.value = doc.value.images.length - 1;
    imageIndex.value = doc.value.images.length - 1;
  } else if (mode.value === "frames" && frame.value) {
    const target = doc.value.modules[bank.value];
    if (!target && !clone) {
      error.value = "Create a module first.";
      return;
    }
    change((s) => {
      s.frames[primary.value].parts.push(
        clone && part.value
          ? { ...part.value }
          : { target: target.id, x: 0, y: 0, flags: 0 },
      );
    });
    secondary.value = frame.value.parts.length - 1;
  } else if (mode.value === "animations" && animation.value) {
    const target = doc.value.frames[bank.value];
    if (!target && !clone) {
      error.value = "Create a frame first.";
      return;
    }
    change((s) =>
      s.animations[primary.value].frames.push(
        clone && aframe.value
          ? { ...aframe.value }
          : { target: target.id, time: 1, x: 0, y: 0, flags: 0 },
      ),
    );
    secondary.value = animation.value.frames.length - 1;
    step.value = animation.value.frames.length - 1;
  }
}
function deleteSecondary() {
  if (!doc.value || !secondaryItems.value.length) return;
  if (
    mode.value === "modules" &&
    !confirm(
      "Delete this image and the modules that use it? This can be undone.",
    )
  )
    return;
  change((s) => {
    if (mode.value === "modules") {
      const removed = new Set(
        s.modules
          .filter(
            (m) => m.type === "MD_IMAGE" && m.values[0] === secondary.value,
          )
          .map((m) => m.id),
      );
      s.images.splice(secondary.value, 1);
      s.modules = s.modules.filter((m) => !removed.has(m.id));
      s.modules.forEach((m) => {
        if (m.type === "MD_IMAGE" && m.values[0] > secondary.value)
          m.values[0]--;
      });
      s.frames.forEach(
        (f) =>
          (f.parts = f.parts.filter(
            (p) => p.flags & 16 || !removed.has(p.target),
          )),
      );
    } else if (mode.value === "frames")
      s.frames[primary.value].parts.splice(secondary.value, 1);
    else s.animations[primary.value].frames.splice(secondary.value, 1);
  });
  secondary.value = Math.max(0, secondary.value - 1);
  imageIndex.value = secondary.value;
  step.value = 0;
}
function moveSecondary(delta: number) {
  const target = secondary.value + delta;
  if (target < 0 || target >= secondaryItems.value.length) return;
  change((s) => {
    const list = (
      mode.value === "modules"
        ? s.images
        : mode.value === "frames"
          ? s.frames[primary.value].parts
          : s.animations[primary.value].frames
    ) as unknown[];
    const [item] = list.splice(secondary.value, 1);
    list.splice(target, 0, item);
    if (mode.value === "modules")
      s.modules.forEach((m) => {
        if (m.type !== "MD_IMAGE") return;
        const i = m.values[0];
        if (i === secondary.value) m.values[0] = target;
        else if (i === target) m.values[0] = secondary.value;
      });
  });
  secondary.value = target;
  if (mode.value === "modules") imageIndex.value = target;
  if (mode.value === "animations") step.value = target;
}
function renameId(value: number) {
  if (!selected.value) return;
  const id = selected.value.id;
  change((s) => {
    s[primaryKind.value][primary.value].id = value;
    if (mode.value === "modules")
      s.frames.forEach((f) =>
        f.parts.forEach((p) => {
          if (!(p.flags & 16) && p.target === id) p.target = value;
        }),
      );
    if (mode.value === "frames") {
      s.animations.forEach((a) =>
        a.frames.forEach((p) => {
          if (p.target === id) p.target = value;
        }),
      );
      s.frames.forEach((f) =>
        f.parts.forEach((p) => {
          if (p.flags & 16 && p.target === id) p.target = value;
        }),
      );
    }
  });
}
function setPart(field: string, value: number) {
  change((s) => {
    const item =
      mode.value === "frames"
        ? s.frames[primary.value].parts[secondary.value]
        : s.animations[primary.value].frames[secondary.value];
    Object.assign(item, { [field]: value });
  });
}
function toggleFlag(flag: number) {
  const p = part.value || aframe.value;
  if (p) setPart("flags", p.flags ^ flag);
}
async function startBuild() {
  if (dirty.value) {
    await save();
    if (opened.value && JSON.stringify(opened.value.document) !== saved.value)
      return;
  }
  job.value = await api("build", {});
  modal.value = "build";
}
function fitPreview() {
  const sprite = doc.value;
  if (!sprite) return;
  const canvas = document.querySelector<HTMLCanvasElement>(
    'canvas[aria-label="Sprite preview"]',
  );
  if (!canvas) return;
  let width = 1,
    height = 1;
  if (mode.value === "modules") {
    if (all.value) {
      width = images.value[imageIndex.value]?.width || 1;
      height = images.value[imageIndex.value]?.height || 1;
    } else if (sprite.modules[primary.value])
      [width, height] = moduleSize(sprite.modules[primary.value]);
  } else {
    const f =
      mode.value === "frames"
        ? sprite.frames[primary.value]
        : sprite.frames.find(
            (f) =>
              f.id ===
              sprite.animations[primary.value]?.frames[step.value]?.target,
          );
    if (f) {
      const r = frameBounds(sprite, f);
      width = 2 * Math.max(Math.abs(r.x1), Math.abs(r.x2), 1);
      height = 2 * Math.max(Math.abs(r.y1), Math.abs(r.y2), 1);
    }
  }
  const size = canvas.getBoundingClientRect();
  zoom.value = Math.max(
    12.5,
    Math.min(
      1600,
      100 *
        Math.min(
          (size.width - 40) / Math.max(width, 1),
          (size.height - 40) / Math.max(height, 1),
        ),
    ),
  );
  viewKey.value = viewKey.value + 1;
}
watch(
  [
    () => opened.value,
    () => mode.value,
    () => dirty.value,
    () => modal.value,
    () => source.value,
    () => sourceBaseline.value,
  ],
  (_values, _old, onCleanup) => {
    const handler = (e: KeyboardEvent) => {
      const editable = (e.target as HTMLElement).closest(
        "input,textarea,select",
      );
      if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === "s") {
        e.preventDefault();
        void run(() => save(e.shiftKey));
        return;
      }
      if (editable) return;
      if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === "z") {
        e.preventDefault();
        restore(e.shiftKey ? "redo" : "undo");
      } else if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === "y") {
        e.preventDefault();
        restore("redo");
      } else if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === "o") {
        e.preventDefault();
        dialogPath.value = opened.value?.path || "";
        modal.value = "open";
      } else if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === "n") {
        e.preventDefault();
        newSprite();
      } else if (e.key === "Escape") {
        modal.value = null;
        menu.value = null;
        draw.value = null;
      } else if (e.key === " " && mode.value === "animations" && !modal.value) {
        e.preventDefault();
        playing.value = !playing.value;
      }
    };
    window.addEventListener("keydown", handler);
    onCleanup(() => window.removeEventListener("keydown", handler));
  },
  { immediate: true, flush: "post" },
);
const menuAction = (action: () => void) => {
  menu.value = null;
  action();
};
const menus = computed(
  (): Record<string, [string, string, () => void][]> => ({
    File: [
      ["New sprite…", "Ctrl+N", newSprite],
      [
        "Open sprite…",
        "Ctrl+O",
        () => {
          dialogPath.value = opened.value?.path || "";
          modal.value = "open";
        },
      ],
      ["Save", "Ctrl+S", () => void run(() => save())],
      ["Save as…", "Ctrl+Shift+S", () => void run(() => save(true))],
      [
        "Export binary sprite…",
        "",
        () => {
          dialogPath.value =
            opened.value?.path.replace(/\.sprite$/i, ".bsprite") || "";
          modal.value = "export";
        },
      ],
      [
        "Run sprite commands…",
        "",
        () => {
          dialogPath.value = defaults.commandFile;
          batchResult.value = "";
          modal.value = "batch";
        },
      ],
      [
        "Exit AuroraMX",
        "",
        () =>
          void run(async () => {
            if (dirty.value && !confirm("Exit without saving?")) return;
            await api("shutdown", {});
            notify("AuroraMX has stopped. You can close this tab.");
          }),
      ],
    ],
    Edit: [
      ["Undo", "Ctrl+Z", () => restore("undo")],
      ["Redo", "Ctrl+Y", () => restore("redo")],
      ["Insert item", "", () => addPrimary()],
      ["Clone item", "", () => addPrimary(true)],
      ["Delete item", "", deletePrimary],
    ],
    View: [
      ["Module Editor", "", () => void run(() => switchMode("modules"))],
      ["Frame Editor", "", () => void run(() => switchMode("frames"))],
      ["Animation Editor", "", () => void run(() => switchMode("animations"))],
      ["Source Editor", "", () => void run(() => switchMode("source"))],
      ["Toggle grid", "", () => (grid.value = !grid.value)],
      ["Toggle origin", "", () => (origin.value = !origin.value)],
    ],
    Tools: [
      ["Build game data…", "", () => void run(startBuild)],
      [
        "Validate sprite",
        "",
        () => {
          const errors = doc.value
            ? validateSprite(doc.value)
            : ["No sprite open"];
          errors.length
            ? (error.value = errors.join("\n"))
            : notify("Sprite is valid.");
        },
      ],
      ["Statistics…", "", () => (modal.value = "stats")],
    ],
    Help: [["AuroraMX help", "", () => (modal.value = "help")]],
  }),
);
const validation = computed(() => (doc.value ? validateSprite(doc.value) : []));
const primaryRows = computed((): Cell[][] =>
  doc.value
    ? mode.value === "modules"
      ? doc.value.modules.map((m, i) => [
          i,
          hex(m.id),
          m.type.replace("MD_", ""),
          ...m.values,
          m.name,
        ])
      : mode.value === "frames"
        ? doc.value.frames.map((f, i) => [
            i,
            hex(f.id),
            f.parts.length,
            f.rects.length,
            f.name,
          ])
        : doc.value.animations.map((a, i) => [
            i,
            hex(a.id),
            a.frames.length,
            a.frames.reduce((n, f) => n + f.time, 0),
            a.name,
          ])
    : [],
);
const columns = computed(() =>
  mode.value === "modules"
    ? ["Index", "ID", "Type", "Image", "X", "Y", "Width", "Height", "Desc"]
    : mode.value === "frames"
      ? ["Index", "ID", "FModules", "Rects", "Desc"]
      : ["Index", "ID", "AFrames", "Ticks", "Desc"],
);
const visiblePrimary = computed(() =>
  filter.value
    ? primaryRows.value
        .map((r, i) => ({ r, i }))
        .filter(({ r }) =>
          r.join(" ").toLowerCase().includes(filter.value.toLowerCase()),
        )
    : primaryRows.value.map((r, i) => ({ r, i })),
);
const secondaryRows = computed((): Cell[][] =>
  doc.value
    ? mode.value === "modules"
      ? doc.value.images.map((i, index) => [
          index,
          hex(i.id),
          images.value[index]
            ? `${images.value[index].width} × ${images.value[index].height}`
            : "…",
          i.transparent === undefined ? "—" : hex(i.transparent),
          i.file,
        ])
      : mode.value === "frames"
        ? (frame.value?.parts || []).map((p, i) => [
            i,
            hex(p.target),
            p.x,
            p.y,
            hex(p.flags),
            doc.value?.modules.find((m) => m.id === p.target)?.name ||
              doc.value?.frames.find((f) => f.id === p.target)?.name ||
              "",
          ])
        : (animation.value?.frames || []).map((p, i) => [
            i,
            hex(p.target),
            p.time,
            p.x,
            p.y,
            hex(p.flags),
            doc.value?.frames.find((f) => f.id === p.target)?.name || "",
          ])
    : [],
);
const secondaryColumns = computed(() =>
  mode.value === "modules"
    ? ["Index", "ID", "Size", "Transp", "File"]
    : mode.value === "frames"
      ? ["Index", "Module / Frame", "X", "Y", "Flags", "Desc"]
      : ["Index", "Frame", "Time", "X", "Y", "Flags", "Desc"],
);

function selectPrimaryRow(index: number) {
  const row = visiblePrimary.value[index];
  if (!row || !doc.value) return;
  primary.value = row.i;
  secondary.value = 0;
  step.value = 0;
  playing.value = false;
  if (mode.value === "modules") {
    const module = doc.value.modules[row.i];
    if (module.values[0] >= 0) imageIndex.value = module.values[0];
  }
}
function togglePlacementFlag(flag: number) {
  const sprite = doc.value,
    placement = part.value;
  if (flag !== 16) {
    toggleFlag(flag);
    return;
  }
  if (!sprite || !placement || !frame.value) return;
  const flags = placement.flags ^ 16;
  const target =
    (flags & 16
      ? sprite.frames.find((f) => f.id !== frame.value!.id)
      : sprite.modules[bank.value]
    )?.id || 0;
  change((s) => {
    const p = s.frames[primary.value].parts[secondary.value];
    p.flags = flags;
    p.target = target;
  });
}
function drawRectangle(rect: SpriteRect) {
  const sprite = doc.value;
  if (!sprite) return;
  if (draw.value === "module") {
    const id = nextId(sprite.modules, 0x1000),
      index = sprite.modules.length;
    change((s) =>
      s.modules.push({
        id,
        type: "MD_IMAGE",
        values: [
          imageIndex.value,
          Math.max(0, rect.x1),
          Math.max(0, rect.y1),
          rect.x2 - rect.x1,
          rect.y2 - rect.y1,
        ],
        name: "",
      }),
    );
    primary.value = index;
  } else change((s) => s.frames[primary.value].rects.push(rect));
  draw.value = null;
}
const previewOptions = computed(() => [
  {
    label: "Grid",
    value: grid.value,
    set: (value: boolean) => {
      grid.value = value;
    },
  },
  {
    label: "Origin",
    value: origin.value,
    set: (value: boolean) => {
      origin.value = value;
    },
  },
  {
    label: "Rectangles",
    value: rects.value,
    set: (value: boolean) => {
      rects.value = value;
    },
  },
  {
    label: "Transparency",
    value: transparency.value,
    set: (value: boolean) => {
      transparency.value = value;
    },
  },
  ...(mode.value === "modules"
    ? [
        {
          label: "All modules",
          value: all.value,
          set: (value: boolean) => {
            all.value = value;
          },
        },
      ]
    : mode.value === "animations"
      ? [
          {
            label: "Onion skin",
            value: onion.value,
            set: (value: boolean) => {
              onion.value = value;
            },
          },
        ]
      : []),
]);
</script>
<template>
  <div class="application" @click="() => menu && (menu = null)">
    <div class="titlebar">
      <span class="app-icon">MX</span> <strong>AuroraMX</strong>
      <span>
        — {{ opened?.path.split("/").pop() || "Sprite Editor" }}
        <template v-if="dirty">{{ " *" }}</template
        ><template v-else>{{ "" }}</template>
      </span>
      <span class="title-note">Sprite Editor</span>
    </div>
    <nav class="menubar" aria-label="Application menu">
      <template
        v-for="([name, items], _index) in Object.entries(menus)"
        :key="name"
        ><div class="menu-wrap">
          <button
            :class="menu === name ? 'active' : ''"
            @click="
              (e) => {
                e.stopPropagation();
                menu = menu === name ? null : name;
              }
            "
          >
            {{ name }}
          </button>
          <template v-if="menu === name"
            ><div class="menu-popup">
              <template
                v-for="([label, key, action], _index) in items"
                :key="label"
                ><button @click="menuAction(action)">
                  {{ label }} <kbd>{{ key }}</kbd>
                </button></template
              >
            </div></template
          >
        </div></template
      >
      <span class="file-path" :title="workspace + '/' + opened?.path">
        {{ opened?.path }}
      </span>
    </nav>
    <div class="toolbar">
      <Tool title="New sprite" @click="newSprite"> <FilePlus /> </Tool>
      <Tool
        title="Open sprite"
        @click="
          () => {
            dialogPath = opened?.path || '';
            modal = 'open';
          }
        "
      >
        <FolderOpen />
      </Tool>
      <Tool
        title="Save sprite"
        :disabled="!doc || busy"
        @click="() => void run(() => save())"
      >
        <Save />
      </Tool>
      <Tool
        title="Export binary sprite"
        :disabled="!doc || busy"
        @click="
          () => {
            dialogPath = opened!.path.replace(/\.sprite$/i, '.bsprite');
            modal = 'export';
          }
        "
      >
        <Download />
      </Tool>
      <i />
      <Tool
        title="Undo"
        :disabled="!undo.length"
        @click="() => restore('undo')"
      >
        <Undo2 />
      </Tool>
      <Tool
        title="Redo"
        :disabled="!redo.length"
        @click="() => restore('redo')"
      >
        <Redo2 />
      </Tool>
      <i />
      <Tool
        title="Module Editor"
        :active="mode === 'modules'"
        @click="() => void run(() => switchMode('modules'))"
      >
        <Grid2X2 />
      </Tool>
      <Tool
        title="Frame Editor"
        :active="mode === 'frames'"
        @click="() => void run(() => switchMode('frames'))"
      >
        <BoxSelect />
      </Tool>
      <Tool
        title="Animation Editor"
        :active="mode === 'animations'"
        @click="() => void run(() => switchMode('animations'))"
      >
        <Play />
      </Tool>
      <i />
      <Tool
        title="Select and move"
        :active="!draw"
        @click="() => (draw = null)"
      >
        <MousePointer2 />
      </Tool>
      <Tool
        :title="
          mode === 'modules'
            ? 'Draw module on atlas'
            : 'Draw collision rectangle'
        "
        :disabled="
          mode === 'animations' ||
          mode === 'source' ||
          (mode === 'frames' && !frame)
        "
        :active="!!draw"
        @click="
          () => {
            all = true;
            draw = mode === 'modules' ? 'module' : 'rect';
          }
        "
      >
        <SquareDashed />
      </Tool>
      <i />
      <Tool
        title="Build game data"
        :disabled="busy || job?.running"
        @click="() => void run(startBuild)"
      >
        <Hammer />
      </Tool>
      <span class="toolbar-caption">Build game data</span>
      <div class="toolbar-spacer" />
      <Tool title="Help" @click="() => (modal = 'help')"> <Info /> </Tool>
    </div>
    <template v-if="error"
      ><div class="error-banner" role="alert">
        <strong>Unable to complete:</strong> {{ error }}
        <button aria-label="Dismiss error" @click="() => (error = '')">
          <X :size="14" />
        </button></div
    ></template>
    <template v-if="!doc"
      ><div class="welcome">
        <div class="app-icon large">MX</div>
        <h1>AuroraMX Sprite Editor</h1>
        <p>Open a sprite or create a new one to begin.</p>
        <button @click="() => (modal = 'open')">
          <FolderOpen :size="16" /> Open sprite…
        </button>
        <button @click="newSprite">New sprite…</button>
      </div></template
    ><template v-else
      ><template v-if="mode === 'source'"
        ><main class="source-editor">
          <header>
            <strong>{{ opened?.path }}</strong>
            <span>Changes are parsed before saving or switching editors.</span>
            <button
              @click="
                () =>
                  void run(async () => {
                    await applySource();
                    notify('Source applied.');
                  })
              "
            >
              Apply source
            </button>
          </header>
          <textarea
            aria-label="Sprite source"
            :value="source"
            @input="(e) => (source = (e.target as HTMLInputElement).value)"
            :spellcheck="false"
          /></main></template
      ><template v-else
        ><main class="editor-grid">
          <div class="left-column">
            <section class="panel primary-panel">
              <header>
                <strong>{{ primaryTitle }}</strong>
                <span>{{ primaryItems.length }} items</span>
                <input
                  :aria-label="'Filter ' + primaryTitle.toLowerCase()"
                  placeholder="Find…"
                  :value="filter"
                  @input="
                    (e) => (filter = (e.target as HTMLInputElement).value)
                  "
                />
              </header>
              <Actions
                :add="() => addPrimary()"
                :clone="() => addPrimary(true)"
                :remove="deletePrimary"
                :up="() => movePrimary(-1)"
                :down="() => movePrimary(1)"
                :disabled="!selected"
              />
              <Table
                :label="primaryTitle"
                :columns="columns"
                :rows="visiblePrimary.map((v) => v.r)"
                :selected="visiblePrimary.findIndex((v) => v.i === primary)"
                @select="selectPrimaryRow"
              />
              <div class="properties">
                <template v-if="selected">
                  <Num
                    label="ID"
                    :value="selected.id"
                    hexadecimal
                    @change="renameId"
                  />
                  <TextField
                    label="Description"
                    :value="selected.name"
                    @change="
                      (v) =>
                        change((s) => {
                          s[primaryKind][primary].name = v;
                        })
                    "
                  />
                  <template v-if="mode === 'modules'">
                    <label class="field">
                      <span>Type</span>
                      <select
                        aria-label="Module type"
                        :value="doc.modules[primary].type"
                        @change="
                          (e) => {
                            const type = (e.target as HTMLInputElement).value;
                            change((s) => {
                              const m = s.modules[primary];
                              m.type = type;
                              m.values =
                                type === 'MD_IMAGE'
                                  ? [0, 0, 0, 32, 32]
                                  : type === 'MD_MARKER'
                                    ? [16, 16]
                                    : type.includes('ARC')
                                      ? [0xffffff, 32, 32, 0, 360]
                                      : type.includes('TRIANGLE')
                                        ? [0xffffff, 32, 0, 16, 32]
                                        : [0xffffff, 32, 32];
                            });
                          }
                        "
                      >
                        <template
                          v-for="(t, _index) in [
                            'MD_IMAGE',
                            'MD_RECT',
                            'MD_FILL_RECT',
                            'MD_MARKER',
                            'MD_ARC',
                            'MD_FILL_ARC',
                            'MD_TRIANGLE',
                            'MD_FILL_TRIANGLE',
                          ]"
                          :key="t"
                          ><option>{{ t }}</option></template
                        >
                      </select>
                    </label>
                    <template
                      v-for="(v, i) in doc.modules[primary].values"
                      :key="i"
                      ><Num
                        :label="
                          (doc.modules[primary].type === 'MD_IMAGE'
                            ? [
                                'Image index',
                                'Module X',
                                'Module Y',
                                'Width',
                                'Height',
                              ]
                            : doc.modules[primary].type === 'MD_MARKER'
                              ? ['Width', 'Height']
                              : doc.modules[primary].type.includes('ARC')
                                ? [
                                    'Color',
                                    'Width',
                                    'Height',
                                    'Start angle',
                                    'Arc angle',
                                  ]
                                : doc.modules[primary].type.includes('TRIANGLE')
                                  ? [
                                      'Color',
                                      'Point 2 X',
                                      'Point 2 Y',
                                      'Point 3 X',
                                      'Point 3 Y',
                                    ]
                                  : ['Color', 'Width', 'Height'])[i]
                        "
                        :value="v"
                        @change="
                          (n) =>
                            change((s) => {
                              s.modules[primary].values[i] = n;
                            })
                        "
                    /></template>
                  </template>
                  <template v-if="mode === 'frames'"
                    ><span class="hint">
                      Double-click a module in the selection bank to add it.
                      Drag a placement in the preview to move it.
                    </span></template
                  >
                  <template v-if="mode === 'animations'"
                    ><span class="hint">
                      AFrame time is measured in ticks. Double-click a frame
                      below to append a step.
                    </span></template
                  >
                </template>
              </div>
            </section>
            <section class="panel secondary-panel">
              <header>
                <strong>{{ secondaryTitle }}</strong>
                <span>{{ secondaryItems.length }} items</span>
              </header>
              <Actions
                :add="() => addSecondary()"
                :clone="() => addSecondary(true)"
                :remove="deleteSecondary"
                :up="() => moveSecondary(-1)"
                :down="() => moveSecondary(1)"
                :disabled="!secondaryItems[secondary]"
              />
              <Table
                :label="secondaryTitle"
                :columns="secondaryColumns"
                :rows="secondaryRows"
                :selected="secondary"
                @select="
                  (index) => {
                    secondary = index;
                    if (mode === 'modules') imageIndex = index;
                    if (mode === 'animations') {
                      step = index;
                      playing = false;
                    }
                  }
                "
              />
              <div class="properties">
                <template v-if="mode === 'modules' && image">
                  <Num
                    label="Image ID"
                    :value="image.id"
                    hexadecimal
                    @change="
                      (v) =>
                        change((s) => {
                          s.images[secondary].id = v;
                        })
                    "
                  />
                  <TextField
                    label="Image file"
                    :value="image.file"
                    @change="
                      (v) =>
                        change((s) => {
                          s.images[secondary].file = v;
                        })
                    "
                  />
                  <TextField
                    label="Alpha file"
                    :value="image.alphaFile || ''"
                    @change="
                      (v) =>
                        change((s) => {
                          s.images[secondary].alphaFile = v || undefined;
                        })
                    "
                  />
                  <Num
                    label="Transparent color"
                    :value="image.transparent ?? 0xff00ff"
                    hexadecimal
                    @change="
                      (v) =>
                        change((s) => {
                          s.images[secondary].transparent = v;
                        })
                    "
                  />
                  <label>
                    <input
                      type="checkbox"
                      :checked="image.transparent !== undefined"
                      @input="
                        (e) =>
                          change((s) => {
                            s.images[secondary].transparent = (
                              e.target as HTMLInputElement
                            ).checked
                              ? 0xff00ff
                              : undefined;
                          })
                      "
                    />
                    Color key
                  </label>
                </template>
                <template v-if="part || aframe">
                  <label class="field wide">
                    <span
                      ><template v-if="part">{{ "Module / Frame" }}</template
                      ><template v-else>{{ "Frame" }}</template></span
                    >
                    <select
                      :aria-label="
                        part ? 'Placement target' : 'Animation frame'
                      "
                      :value="(part || aframe)!.target"
                      @change="
                        (e) =>
                          setPart(
                            'target',
                            Number((e.target as HTMLInputElement).value),
                          )
                      "
                    >
                      <template
                        v-for="(m, i) in part
                          ? part.flags & 16
                            ? doc.frames
                            : doc.modules
                          : doc.frames"
                        :key="m.id"
                        ><option :value="m.id">
                          {{ i }} · {{ hex(m.id) }} {{ m.name }}
                        </option></template
                      >
                    </select>
                  </label>
                  <Num
                    label="Offset X"
                    :value="(part || aframe)!.x"
                    @change="(v) => setPart('x', v)"
                  />
                  <Num
                    label="Offset Y"
                    :value="(part || aframe)!.y"
                    @change="(v) => setPart('y', v)"
                  />
                  <template v-if="aframe"
                    ><Num
                      label="Time"
                      :value="aframe.time"
                      @change="(v) => setPart('time', v)"
                  /></template>
                  <Num
                    label="Flags"
                    :value="(part || aframe)!.flags"
                    hexadecimal
                    @change="(v) => setPart('flags', v)"
                  />
                  <div class="flag-row">
                    <template
                      v-for="([label, flag], _index) in [
                        ['Flip X', 1],
                        ['Flip Y', 2],
                        ['Rotate 90\u00B0', 4],
                        ...(part ? [['Hyperframe', 16]] : []),
                      ]"
                      :key="label"
                      ><label>
                        <input
                          type="checkbox"
                          :checked="!!((part || aframe)!.flags & Number(flag))"
                          @input="togglePlacementFlag(Number(flag))"
                        />
                        {{ label }}
                      </label></template
                    >
                  </div>
                </template>
              </div>
              <template v-if="frame"
                ><div class="collision-editor">
                  <div class="subheading">
                    <strong>Collision rectangles</strong>
                    <button
                      @click="
                        () =>
                          change((s) => {
                            s.frames[primary].rects.push({
                              x1: -16,
                              y1: -16,
                              x2: 16,
                              y2: 16,
                            });
                          })
                      "
                    >
                      + Add rectangle
                    </button>
                  </div>
                  <template v-for="(r, i) in frame.rects" :key="i"
                    ><div class="rect-fields">
                      <span>RC {{ i }}</span>
                      <template
                        v-for="(key, _index) in [
                          'x1',
                          'y1',
                          'x2',
                          'y2',
                        ] as const"
                        :key="key"
                        ><Num
                          :label="`RC ${i} ${key}`"
                          :value="r[key]"
                          @change="
                            (v) =>
                              change((s) => {
                                s.frames[primary].rects[i][key] = v;
                              })
                          "
                      /></template>
                      <button
                        title="Delete rectangle"
                        :aria-label="`Delete rectangle ${i}`"
                        @click="
                          () =>
                            change((s) => {
                              s.frames[primary].rects.splice(i, 1);
                            })
                        "
                      >
                        <X :size="13" />
                      </button></div
                  ></template></div
              ></template>
            </section>
          </div>
          <div class="right-column">
            <section class="panel preview-panel">
              <header>
                <strong>
                  <template v-if="mode === 'modules'">{{ "Module" }}</template
                  ><template v-else
                    ><template v-if="mode === 'frames'">{{ "Frame" }}</template
                    ><template v-else>{{ "Animation" }}</template></template
                  >{{ " " }} preview
                </strong>
                <span>
                  {{
                    selected?.name ||
                    (selected && hex(selected.id)) ||
                    "No selection"
                  }}
                </span>
              </header>
              <div class="preview-controls">
                <label v-for="option in previewOptions" :key="option.label">
                  <input
                    type="checkbox"
                    :checked="option.value"
                    @change="
                      option.set(($event.target as HTMLInputElement).checked)
                    "
                  />
                  {{ option.label }}
                </label>
                <div class="zoom-controls">
                  <button aria-label="Fit preview" @click="fitPreview">
                    Fit
                  </button>
                  <button
                    aria-label="Zoom out"
                    @click="() => (zoom = Math.max(12.5, zoom / 1.25))"
                  >
                    <ZoomOut :size="14" />
                  </button>
                  <select
                    aria-label="Zoom"
                    :value="zoom"
                    @change="
                      (e) =>
                        (zoom = Number((e.target as HTMLInputElement).value))
                    "
                  >
                    <template
                      v-for="(n, _index) in [
                        ...new Set([
                          25,
                          50,
                          75,
                          100,
                          150,
                          200,
                          300,
                          400,
                          800,
                          1600,
                          zoom,
                        ]),
                      ].sort((a, b) => a - b)"
                      :key="n"
                      ><option :value="n">
                        {{ Math.round(n) }}%
                      </option></template
                    >
                  </select>
                  <button
                    aria-label="Zoom in"
                    @click="() => (zoom = Math.min(1600, zoom * 1.25))"
                  >
                    <ZoomIn :size="14" />
                  </button>
                  <button @click="() => (zoom = 100)">1:1</button>
                </div>
              </div>
              <template v-if="imageError"
                ><div class="image-error">{{ imageError }}</div></template
              >
              <PreviewCanvas
                :viewKey="viewKey"
                :sprite="doc"
                :images="images"
                :mode="mode"
                :primary="primary"
                :secondary="secondary"
                :imageIndex="imageIndex"
                :step="step"
                :zoom="zoom"
                :grid="grid"
                :origin="origin"
                :rects="rects"
                :all="all"
                :onion="onion"
                :draw="draw"
                @select="
                  (index) => {
                    if (mode === 'modules') primary = index;
                    else secondary = index;
                  }
                "
                @drag-start="remember"
                @move="
                  (i, x, y) =>
                    change((s) => {
                      if (mode === 'modules') {
                        s.modules[i].values[1] = Math.max(0, x);
                        s.modules[i].values[2] = Math.max(0, y);
                      } else {
                        s.frames[primary].parts[i].x = x;
                        s.frames[primary].parts[i].y = y;
                      }
                    }, false)
                "
                @draw="drawRectangle"
                @zoom="(value) => (zoom = value)"
                @cursor="(x, y) => (cursor = { x, y })"
              />
              <template v-if="mode === 'animations'"
                ><div class="playback">
                  <button
                    aria-label="First animation frame"
                    @click="
                      () => {
                        step = 0;
                        secondary = 0;
                        playing = false;
                      }
                    "
                  >
                    <SkipBack :size="15" />
                  </button>
                  <button
                    :aria-label="playing ? 'Pause animation' : 'Play animation'"
                    :disabled="!animation?.frames.length"
                    @click="() => (playing = !playing)"
                  >
                    <template v-if="playing"><Pause :size="15" /></template
                    ><template v-else><Play :size="15" /></template>
                  </button>
                  <button
                    aria-label="Next animation frame"
                    :disabled="!animation?.frames.length"
                    @click="
                      () => {
                        const n = (step + 1) % (animation?.frames.length || 1);
                        step = n;
                        secondary = n;
                        playing = false;
                      }
                    "
                  >
                    <SkipForward :size="15" />
                  </button>
                  <input
                    aria-label="Animation position"
                    type="range"
                    min="0"
                    :max="Math.max(0, (animation?.frames.length || 1) - 1)"
                    :value="step"
                    @input="
                      (e) => {
                        const i = Number((e.target as HTMLInputElement).value);
                        step = i;
                        secondary = i;
                        playing = false;
                      }
                    "
                  />
                  <span>
                    Step
                    <template v-if="animation?.frames.length">{{
                      step + 1
                    }}</template
                    ><template v-else>{{ 0 }}</template> /{{ " " }}
                    {{ animation?.frames.length || 0 }}
                  </span>
                  <label>
                    FPS{{ " " }}
                    <input
                      aria-label="Animation FPS"
                      type="number"
                      min="1"
                      max="120"
                      :value="fps"
                      @input="
                        (e) =>
                          (fps = Math.min(
                            120,
                            Math.max(
                              1,
                              Number((e.target as HTMLInputElement).value),
                            ),
                          ))
                      "
                    />
                  </label>
                  <label>
                    <input
                      type="checkbox"
                      :checked="loop"
                      @input="
                        (e) => (loop = (e.target as HTMLInputElement).checked)
                      "
                    />
                    Loop
                  </label>
                </div></template
              >
            </section>
            <section class="panel bank-panel">
              <header>
                <strong>
                  <template v-if="mode === 'modules'">{{ "Images" }}</template
                  ><template v-else
                    ><template v-if="mode === 'frames'">{{
                      "Modules"
                    }}</template
                    ><template v-else>{{ "Frames" }}</template></template
                  >{{ " " }} · selection bank
                </strong>
                <span>
                  <template v-if="mode === 'modules'">{{
                    "Click an image to view its atlas"
                  }}</template
                  ><template v-else>{{ "Double-click to insert" }}</template>
                </span>
              </header>
              <div class="bank">
                <template v-if="mode === 'modules'"
                  ><template v-for="(i, index) in doc.images" :key="i.id"
                    ><button
                      :class="
                        'image-tile ' + (imageIndex === index ? 'selected' : '')
                      "
                      @click="
                        () => {
                          imageIndex = index;
                          secondary = index;
                          all = true;
                        }
                      "
                    >
                      <template v-if="images[index]"
                        ><img :src="images[index].src" /></template
                      ><template v-else><span>Loading…</span></template>
                      <span>
                        {{ index }}: {{ i.file.split(/[\\/]/).pop() }}
                      </span>
                    </button></template
                  ></template
                ><template v-else
                  ><template
                    v-for="(m, i) in mode === 'frames'
                      ? doc.modules
                      : doc.frames"
                    :key="m.id"
                    ><Thumbnail
                      :sprite="doc"
                      :images="images"
                      :kind="mode === 'frames' ? 'module' : 'frame'"
                      :index="i"
                      :selected="bank === i"
                      @click="() => (bank = i)"
                      @dblclick="
                        () => {
                          if (mode === 'frames')
                            change((s) =>
                              s.frames[primary]?.parts.push({
                                target: m.id,
                                x: 0,
                                y: 0,
                                flags: 0,
                              }),
                            );
                          else
                            change((s) =>
                              s.animations[primary]?.frames.push({
                                target: m.id,
                                time: 1,
                                x: 0,
                                y: 0,
                                flags: 0,
                              }),
                            );
                          bank = i;
                        }
                      " /></template
                ></template>
              </div>
            </section>
          </div></main></template
    ></template>
    <div class="editor-tabs" role="tablist">
      <template
        v-for="([key, label], _index) in Object.entries(modeLabels)"
        :key="key"
        ><button
          role="tab"
          :aria-selected="mode === key"
          :class="mode === key ? 'active' : ''"
          :disabled="!doc || busy"
          @click="() => void run(() => switchMode(key as EditorMode))"
        >
          {{ label }}
        </button></template
      >
    </div>
    <footer class="statusbar">
      <span>
        <template v-if="draw"
          ><template v-if="draw === 'module'">{{
            "Drag on the atlas to create a module."
          }}</template
          ><template v-else>{{
            "Drag in the preview to create a collision rectangle."
          }}</template></template
        ><template v-else
          ><template v-if="validation.length">{{
            `${validation.length} validation issue(s): ${validation[0]}`
          }}</template
          ><template v-else
            ><template v-if="busy">{{ "Working…" }}</template
            ><template v-else>{{ message }}</template></template
          ></template
        >
      </span>
      <span
        ><template v-if="dirty">{{ "Modified" }}</template
        ><template v-else>{{ "Saved" }}</template></span
      >
      <span> X: {{ cursor.x }} Y: {{ cursor.y }} </span>
      <span>
        {{ doc?.modules.length || 0 }} modules ·
        {{ doc?.frames.length || 0 }} frames ·
        {{ doc?.animations.length || 0 }} anims
      </span>
    </footer>
    <template v-if="modal"
      ><div
        class="modal-shade"
        @mousedown="
          (e) => {
            if (e.target === e.currentTarget && modal !== 'build') modal = null;
          }
        "
      >
        <section
          :class="
            'dialog ' +
            (modal === 'open' || modal === 'build' ? 'large-dialog' : '')
          "
          role="dialog"
          aria-modal="true"
          :aria-label="
            {
              open: 'Open sprite',
              new: 'New sprite',
              saveAs: 'Save sprite as',
              export: 'Export binary sprite',
              batch: 'Run sprite commands',
              build: 'Build game data',
              help: 'AuroraMX help',
              stats: 'Sprite statistics',
            }[modal]
          "
        >
          <header>
            <strong>
              {{
                {
                  open: "Open sprite",
                  new: "New sprite",
                  saveAs: "Save sprite as",
                  export: "Export binary sprite",
                  batch: "Run sprite commands",
                  build: "Build game data",
                  help: "AuroraMX help",
                  stats: "Sprite statistics",
                }[modal]
              }}
            </strong>
            <button aria-label="Close dialog" @click="() => (modal = null)">
              <X :size="15" />
            </button>
          </header>
          <div class="dialog-body">
            <template v-if="modal === 'open'">
              <div class="folder-path">
                <button
                  aria-label="Parent folder"
                  @click="
                    () =>
                      (directory = directory.includes('/')
                        ? directory.slice(0, directory.lastIndexOf('/'))
                        : '.')
                  "
                >
                  <ArrowUp :size="14" />
                </button>
                <span :title="workspace">{{ workspace }}</span>
                <ChevronRight :size="13" /> <strong>{{ directory }}</strong>
              </div>
              <div class="file-browser">
                <template v-for="(e, _index) in entries" :key="e.path"
                  ><button
                    :class="dialogPath === e.path ? 'selected' : ''"
                    @click="
                      () =>
                        e.directory
                          ? (directory = e.path)
                          : (dialogPath = e.path)
                    "
                    @dblclick="
                      () => {
                        if (!e.directory && e.name.endsWith('.sprite'))
                          void run(() => openFile(e.path));
                      }
                    "
                  >
                    <template v-if="e.directory"><Folder :size="18" /></template
                    ><template v-else><FileCode2 :size="18" /></template>
                    <span>{{ e.name }}</span>
                    <small>
                      <template v-if="e.directory">{{ "Folder" }}</template
                      ><template v-else>{{
                        e.name.split(".").pop()?.toUpperCase()
                      }}</template>
                    </small>
                  </button></template
                >
              </div>
              <label class="dialog-field">
                File name
                <input
                  aria-label="Open file path"
                  :value="dialogPath"
                  @input="
                    (e) => (dialogPath = (e.target as HTMLInputElement).value)
                  "
                  @keydown="
                    (e) => {
                      if (e.key === 'Enter')
                        void run(() => openFile(dialogPath));
                    }
                  "
                />
              </label>
              <div class="dialog-actions">
                <span>Sprite files (*.sprite)</span>
                <button @click="() => (modal = null)">Cancel</button>
                <button
                  class="default"
                  :disabled="busy || !dialogPath.endsWith('.sprite')"
                  @click="() => void run(() => openFile(dialogPath))"
                >
                  Open
                </button>
              </div>
            </template>
            <template v-if="modal === 'new' || modal === 'saveAs'">
              <p>Choose a .sprite path relative to the workspace.</p>
              <label class="dialog-field">
                File name
                <input
                  autofocus
                  aria-label="Save file path"
                  :value="dialogPath"
                  @input="
                    (e) => (dialogPath = (e.target as HTMLInputElement).value)
                  "
                />
              </label>
              <div class="dialog-actions">
                <button @click="() => (modal = null)">Cancel</button>
                <button
                  class="default"
                  :disabled="busy || !dialogPath.endsWith('.sprite')"
                  @click="
                    () =>
                      void run(async () => {
                        if (modal === 'new') {
                          const document = emptySprite();
                          install({
                            path: dialogPath,
                            document,
                            source: serializeSprite(document),
                            revision: '',
                          });
                          saved = '';
                          modal = null;
                          notify(
                            'New sprite. Add an image and modules to begin.',
                          );
                        } else {
                          const document = await applySource();
                          const value = await api<OpenSprite>('save', {
                            path: dialogPath,
                            document,
                            revision: '',
                            saveAs: true,
                            sourcePath: opened!.path,
                            rawSource:
                              appliedSource?.document ===
                              JSON.stringify(document)
                                ? appliedSource.text
                                : undefined,
                          });
                          install(value);
                          modal = null;
                          notify('Saved ' + value.path);
                        }
                      })
                  "
                >
                  <template v-if="modal === 'new'">{{ "Create" }}</template
                  ><template v-else>{{ "Save" }}</template>
                </button>
              </div>
            </template>
            <template v-if="modal === 'export'">
              <p>
                Export a game-compatible binary sprite using the GP_Data
                profile. The texture atlas stays separate.
              </p>
              <label class="dialog-field">
                Output file
                <input
                  aria-label="Binary output path"
                  :value="dialogPath"
                  @input="
                    (e) => (dialogPath = (e.target as HTMLInputElement).value)
                  "
                />
              </label>
              <div class="dialog-actions">
                <button @click="() => (modal = null)">Cancel</button>
                <button
                  class="default"
                  :disabled="busy"
                  @click="
                    () =>
                      void run(async () => {
                        const document = await applySource();
                        const result = await api('export', {
                          path: opened!.path,
                          document,
                          output: dialogPath,
                        });
                        modal = null;
                        notify(
                          `Exported ${result.path} (${result.bytes} bytes)`,
                        );
                      })
                  "
                >
                  Export .bsprite
                </button>
              </div>
            </template>
            <template v-if="modal === 'batch'">
              <p>
                Run the project's sprite command file to export binary sprites
                and C/C++ frame and animation IDs.
              </p>
              <label class="dialog-field">
                Command file
                <input
                  aria-label="Command file"
                  :value="dialogPath"
                  @input="
                    (e) => (dialogPath = (e.target as HTMLInputElement).value)
                  "
                />
              </label>
              <label class="dialog-field">
                Output directory
                <input
                  aria-label="Batch output directory"
                  :value="outputPath"
                  @input="
                    (e) => (outputPath = (e.target as HTMLInputElement).value)
                  "
                />
              </label>
              <template v-if="batchResult">
                <pre class="build-log">{{ batchResult }}</pre>
              </template>
              <div class="dialog-actions">
                <button @click="() => (modal = null)">Close</button>
                <button
                  class="default"
                  :disabled="busy"
                  @click="
                    () =>
                      void run(async () => {
                        const result = await api('batch', {
                          path: dialogPath,
                          output: outputPath,
                        });
                        batchResult =
                          result.sprites
                            .map(
                              (s: { output: string; bytes: number }) =>
                                s.output + ' (' + s.bytes + ' bytes)',
                            )
                            .join('\n') +
                          `\nExported ${result.sprites.length} sprites.`;
                        notify(`Exported ${result.sprites.length} sprites.`);
                      })
                  "
                >
                  Run commands
                </button>
              </div>
            </template>
            <template v-if="modal === 'build'">
              <p>
                <template v-if="job?.running">{{
                  "Building current game resources…"
                }}</template
                ><template v-else
                  ><template v-if="job?.exitCode === 0">{{
                    "Game resources built successfully."
                  }}</template
                  ><template v-else>{{
                    "Resource build finished."
                  }}</template></template
                >
              </p>
              <pre class="build-log" role="log">
 {{ job?.log || "Starting…" }} </pre
              >
              <div class="dialog-actions">
                <button @click="() => (modal = null)">Close</button>
                <button
                  :disabled="busy || job?.running"
                  @click="() => void run(startBuild)"
                >
                  Build again
                </button>
              </div>
            </template>
            <template v-if="modal === 'help'"
              ><div class="help">
                <h2>AuroraMX Sprite Editor</h2>
                <p>A browser edition of the AuroraGT sprite workflow.</p>
                <p>
                  <b>Module Editor:</b> add an image, select or draw atlas
                  rectangles, and edit module coordinates and sizes.
                </p>
                <p>
                  <b>Frame Editor:</b> double-click modules in the bank, drag
                  placements, change flags and layer order, and edit collision
                  rectangles.
                </p>
                <p>
                  <b>Animation Editor:</b> double-click frames to add steps. Set
                  durations and offsets, then play, pause, scrub or preview
                  onion skin.
                </p>
                <p>
                  <b>Source Editor:</b> edit the original sprite language and
                  apply it before saving. File backups are stored in
                  .auroramx/backups.
                </p>
                <p>
                  <b>Export:</b> GP_Data .bsprite output, or run Sprites.sprcmd
                  for all sprites and headers. Build game data runs the Linux
                  resource pipeline.
                </p>
                <p>
                  <b>Shortcuts:</b> Ctrl+O open · Ctrl+N new · Ctrl+S save ·
                  Ctrl+Shift+S save as · Ctrl+Z/Y undo/redo · Space play/pause ·
                  Alt+drag pan · Wheel zoom.
                </p>
                <p class="muted">
                  AuroraMX is an independent implementation. The original
                  AuroraGT help screenshots informed this layout. GameEditor and
                  MapEditor are separate tools.
                </p>
              </div></template
            >
            <template v-if="modal === 'stats' && doc"
              ><div class="help">
                <h2>Sprite statistics</h2>
                <p>
                  {{ doc.images.length }} images ·
                  {{ doc.modules.length }} modules ·{{ " " }}
                  {{ doc.frames.length }} frames · {{ doc.animations.length
                  }}{{ " " }} animations
                </p>
                <p>
                  {{ doc.frames.reduce((n, f) => n + f.parts.length, 0)
                  }}{{ " " }} FModules ·{{ " " }}
                  {{ doc.animations.reduce((n, a) => n + a.frames.length, 0)
                  }}{{ " " }} AFrames ·{{ " " }}
                  {{ doc.frames.reduce((n, f) => n + f.rects.length, 0)
                  }}{{ " " }} collision rectangles
                </p>
                <p>
                  <template v-if="validation.length">{{
                    validation.join("\n")
                  }}</template
                  ><template v-else>{{ "No validation issues." }}</template>
                </p>
              </div></template
            >
            <template v-if="error"
              ><div class="dialog-error" role="alert">
                {{ error }}
              </div></template
            >
          </div>
        </section>
      </div></template
    >
  </div>
</template>
