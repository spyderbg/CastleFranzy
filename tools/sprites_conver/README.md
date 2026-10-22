# Converting the legacy sprites to YAML

The sprite editor in `Tools/AuroraMX` reads the old AuroraGT `*.sprite` documents
and saves them as YAML. The game loads both formats: `Src/Sprites/ASprite.cpp`
prefers a `.yaml` (or `.yml`) next to the `.sprite` file, and
`Src/Sprites/SpriteYaml.cpp` compiles it into the same binary sprite.

This tool converts the files outside the editor. It reads the legacy format,
writes the YAML format and checks the result against the rules of the game and
of the editor.

Python 3 is enough for the tool. PyYAML is used by the test suite when it is
installed. The editor check needs the AuroraMX command line tool (see below).

## Usage

```bash
# one file, written next to the source
python3 tools/sprites_opencode/convert.py convert Data/Sprites/Ingame/effects.sprite

# a whole tree, written into another directory
python3 tools/sprites_opencode/convert.py convert Data/Sprites --output build/yaml

# back to the legacy format
python3 tools/sprites_opencode/convert.py reverse build/yaml --output build/sprite

# read a document and report what is wrong with it
python3 tools/sprites_opencode/convert.py verify Data/Sprites/Menu/menu.sprite

# a short summary of a document
python3 tools/sprites_opencode/convert.py info Data/Sprites/Menu/menu.sprite
```

Options of `convert` and `reverse`:

| Option | Meaning |
| --- | --- |
| `-o`, `--output DIR` | write the results into `DIR` instead of next to the source |
| `--overwrite` | replace files that already exist (by default they are never touched) |
| `--no-verify` | skip the read back check of the written file |
| `--auroramx` | load the result with the AuroraMX command line tool as well |
| `-q`, `--quiet` | hide the progress and the notes; problems are still reported |

Every conversion is checked: the written YAML is read back and compared with the
source document, and the document is validated against the rules of the game's
YAML loader. A conversion that does not survive the round trip is an error. A
problem is reported even with `--quiet`, and the command fails. The file is
still written.

`verify` reports the document as it is, so it also reports problems that
`convert` resolves, such as the module without an image in `fireball.sprite`.

## The editor check

`--auroramx` loads the result with `Tools/AuroraMX/build/cli.cjs`. The bundle is
a build artifact that is not committed, so build it once:

```bash
cd Tools/AuroraMX
npm ci
npm run build:cli
```

If the bundle is missing or older than the YAML support of the editor, the tool
says so. The test suite loads every project sprite with the bundle when it is
built.

## The format

The output is the mapping under a top level `sprite` key, as AuroraMX writes it:

```yaml
sprite:
  version: "0001"

  images:
    - id: "0x0000"
      file: "./effects.tga"
      transparent: "0x00FF00FF"

  modules:
    - id: "0x1000"
      type: MD_IMAGE
      image: "0x0000"
      x: 3
      y: 5
      w: 7
      h: 7

  frames:
    - id: "0x2000"
      name: ""
      parts:
        - { module: "0x1000", x: -13, y: 7 }

  animations:
    - id: "0x3000"
      name: "dizzy"
      frames:
        - { frame: "0x2000", time: 2, x: 0, y: 0 }
```

Three details are worth knowing:

* **IDs are quoted hexadecimal strings** such as `"0x1000"`. The game only accepts
  IDs of that shape (`Src/Sprites/SpriteYaml.cpp`).
* **A placement always uses the `module` key.** A hyperframe is a placement with
  the `HYPER_FM` flag: `{ module: "0x2000", x: 0, y: 0, flags: [HYPER_FM] }`. The
  game reads `module` for every frame part. The editor writes the same form.
* **Flags are names.** `FLIP_X`, `FLIP_Y`, `ROT_90` and `HYPER_FM` are written as a
  list. Bits without a name, such as the legacy `FREE_ROT`, are written as
  `extraFlags`. The game ignores that key, so such bits are reported as problems.

## What the converter changes

The legacy format and the YAML format disagree in a few places. The converter
resolves them and prints a note for each change that matters.

* **Image IDs are renumbered.** The legacy documents number their images freely,
  but the modules refer to an image by its *index*. The editor resolves a module's
  image by index, the game by *ID*. Numbering the images after their index makes
  both agree. The notes list every renumbered image.
* **A module without an image uses the first image.** `fireball.sprite` contains
  the module `0x101A` with the image index `-1`, and no frame uses it. The game's
  YAML loader rejects a module whose image does not exist, so the converter points
  it at the first image and prints a note. The editor's binary export does not
  store the image index, so the picture does not change. A module that a frame
  does use is reported as a problem, because the picture can change.
* **Image paths use forward slashes.** The legacy documents store Windows paths
  such as `.\effects.tga`, the YAML files use `./effects.tga`.
* **Everything else is kept**: names, collision rectangles, module positions,
  animation times and the order of all lists.

## Publishing the result

The game prefers a `.yaml` over the binary export of the same sprite, so a
converted file goes live as soon as it sits next to its `.sprite`. Convert into a
review folder first, check the sprites in the game, and copy the files over
afterwards:

```bash
python3 tools/sprites_opencode/convert.py convert Data/Sprites --output build/yaml
```

Do not convert in place with `--overwrite` on `Data/Sprites`:
`Data/Sprites/Ingame/dizzy_effect.yaml` is a hand-written, tracked file.

## The rules of the game

`validate_game_rules()` mirrors the loader of the game so that a converted
document is known to load:

* the version is `"0001"`, a sprite needs an image, a module and a frame;
* all IDs are unique and fit into the 16 bits the game uses;
* the modules are `MD_IMAGE` modules that all use the same atlas, with a
  position and a size between 0 and 32767;
* a frame has at most 255 collision rectangles, a placement an offset between
  -32768 and 32767, and the computed bounds of the frame stay inside the
  runtime limits;
* an animation step refers to one of the first 1024 frames, its time is between
  0 and 255 and its offset between -128 and 127. The `HYPER_FM` flag is not
  allowed in an animation;
* placements and steps carry no flag bits the game does not read.

## Verification

```bash
python3 tools/sprites_opencode/test_convert.py
```

The suite covers the legacy reader and writer, the YAML reader and writer, the
game rules and the conversion of every sprite of the project. It checks that all
29 documents of `Data/Sprites` convert and survive the round trip. When the
AuroraMX bundle is built, it also loads every converted sprite with that tool.

To compare with the hand-written reference, convert `dizzy_effect.sprite` and
diff the result. The differences are comments and column alignment only.

## The files

| File | Contents |
| --- | --- |
| `convert.py` | the command line tool |
| `sprite_model.py` | the document model, the flag tables and the game rules |
| `sprite_text.py` | the reader and the writer of the legacy `.sprite` format |
| `yaml_lite.py` | a small YAML reader for the subset the sprite format uses |
| `sprite_yaml.py` | the reader and the writer of the AuroraMX YAML format |
| `test_convert.py` | the test suite |
