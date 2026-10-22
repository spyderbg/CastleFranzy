export const challenges = [
  "None",
  "PerfectHealth",
  "GoblinSmasher",
  "ShamanSmasher",
  "BoarRampage",
  "GriffinFireballMaster",
  "GriffinFireballSuperMaster",
  "FlickMaster",
  "OneHitOneKill",
  "ManyEnemiesWithSpecial",
  "ThatWasClose",
  "WallSmasher",
  "GroundSmasher",
  "MenuGoblins",
  "MenuTrolls",
  "MenuShamans",
  "ShamanCombo",
  "TrollCombo",
  "ComboMaster",
];
export const weapons = ["Gate", "Hydra", "Fireball", "Iceball", "Griffin"];
export const spells = ["None", "Boulder", "Lightning", "Ghost army", "Tornado"];
export interface Enemy {
  type: string;
  min: number;
  max: number;
}
export interface Variant {
  intro: boolean;
  enemies: Enemy[];
}
export interface Wave {
  name: string;
  timeStart: number;
  timeInterval: number;
  repeatInterval: number;
  repeatCount: number;
  packed: boolean;
  variants: Variant[];
}
export interface Day {
  dayEndTime: number;
  challenge: string;
  difficulty: number;
  waves: Wave[];
}
export interface Loadout {
  weapons: { startingLevel: number; shop: boolean }[];
  spell: number;
  spellBought: boolean;
}
export interface Level {
  loadout: Loadout;
  days: Day[];
}
export interface Project {
  levels: Level[];
}
export interface Unit {
  name: string;
  hidden: boolean;
  icon: number[];
  model: string;
}
export interface Model {
  name: string;
  file: string;
  scale: number;
  attributes: Record<string, string>;
}
export interface Clip {
  name: string;
  start: number;
  end: number;
}
export interface LiveUnit {
  time: number;
  type: number;
  state: "dead" | "alive" | "next" | "future";
  extra?: boolean;
}
export interface GameStatus {
  running: boolean;
  ready?: boolean;
  valid?: boolean;
  paused?: boolean;
  level?: number;
  day?: number;
  time?: number;
  gateHealth?: number;
  loadingProgress?: number;
  seq?: number;
  error?: string;
  units?: LiveUnit[];
  log?: string;
}
export interface OpenProject {
  project: Project;
  revision: string;
  assets: string;
  assetRoots: string[];
  units: Unit[];
  models: Model[];
}
export const clone = <T>(value: T): T => JSON.parse(JSON.stringify(value));
export const newDay = (): Day => ({
  dayEndTime: 100,
  challenge: "None",
  difficulty: 0,
  waves: [],
});
export const newWave = (index: number): Wave => ({
  name: `wave${index + 1}`,
  timeStart: 0,
  timeInterval: 5,
  repeatInterval: 20,
  repeatCount: 1,
  packed: true,
  variants: [],
});
export const newLevel = (): Level => ({
  loadout: {
    weapons: weapons.map(() => ({ startingLevel: -1, shop: true })),
    spell: -1,
    spellBought: false,
  },
  days: [],
});

export function validate(project: Project, units: Unit[]): void {
  const fail = (message: string): never => {
    throw new Error(message);
  };
  const array = (value: unknown, max: number): void => {
    if (!Array.isArray(value) || value.length > max)
      fail("Invalid or oversized collection");
  };
  const int = (n: number, min: number, max: number): void => {
    if (!Number.isInteger(n) || n < min || n > max)
      fail(`Expected integer ${min}..${max}, got ${n}`);
  };
  const bool = (n: unknown): void => {
    if (typeof n !== "boolean") fail("Expected boolean");
  };
  array(project.levels, 99);
  for (const level of project.levels) {
    array(level.days, 99);
    array(level.loadout.weapons, 5);
    if (level.loadout.weapons.length !== 5) fail("Expected five weapons");
    for (const weapon of level.loadout.weapons) {
      int(weapon.startingLevel, -1, 2);
      bool(weapon.shop);
    }
    int(level.loadout.spell, -1, 3);
    bool(level.loadout.spellBought);
    for (const day of level.days) {
      int(day.dayEndTime, 1, 999);
      int(day.difficulty, 0, 2);
      if (!challenges.includes(day.challenge)) fail("Unknown challenge");
      array(day.waves, 1000);
      for (const wave of day.waves) {
        if (
          typeof wave.name !== "string" ||
          wave.name.length > 15 ||
          /[\u0000-\u0008\u000b\u000c\u000e-\u001f]/.test(wave.name)
        )
          fail("Wave name must be at most 15 characters");
        int(wave.timeStart, 0, 999);
        int(wave.timeInterval, 0, 999);
        int(wave.repeatInterval, 0, 999);
        int(wave.repeatCount, 1, 99);
        bool(wave.packed);
        array(wave.variants, 100);
        for (const variant of wave.variants) {
          bool(variant.intro);
          array(variant.enemies, 100);
          for (const enemy of variant.enemies) {
            if (!units.some((u) => u.name === enemy.type))
              fail(`Unknown unit: ${enemy.type}`);
            int(enemy.min, 1, 99);
            int(enemy.max, enemy.min, 99);
          }
        }
      }
    }
  }
}
