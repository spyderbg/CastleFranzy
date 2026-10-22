# Converting the legacy sprites to YAML

The sprite editor in `Tools/AuroraMX` reads the old AuroraGT `*.sprite` documents
of the game and saves them as YAML. The game loads both formats: `Src/Sprites/ASprite.cpp`
prefers a `*.yaml` next to the `*.sprite` file, and `Src/Sprites/SpriteYaml.cpp`
compiles it into the same binary sprite the old format produced.

This tool performs that conversion outside the editor. It reads the legacy format,
writes the AuroraMX YAML format and checks the result against the rules of both
readers, so a converted file can be opened in AuroraMX and loaded by the game.

Only Python 3 is needed; there are no third party packages. PyYAML is used by the
test suite when it happens to be installed, but the tool itself does not need it.

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
| `--image-ids normalize\|keep` | number the images after their index (default) or keep the legacy IDs |
| `--no-verify` | skip the read back check of the written file |
| `--auroramx` | load the result with the AuroraMX command line tool as well |
| `-q`, `--quiet` | only report problems |

Every conversion is checked: the written YAML is read back and compared with the
source document, and the document is validated against the rules of the game's
YAML loader. A conversion that does not survive the round trip is an error, not a
warning.

The `--auroramx` option asks `Tools/AuroraMX/build/cli.cjs` to load the result.
The bundle in the repository is older than the YAML support of the editor; when it
cannot read YAML the tool says so and asks for `npm run build:cli` in
`Tools/AuroraMX`. The check is optional, the conversion does not depend on it.

## The format

The output is the mapping under a top level `sprite` key that AuroraMX writes:

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
  IDs of that shape (`Src/Sprites/SpriteYaml.cpp`), the editor accepts plain
  numbers as well.
* **A placement always uses the `module` key.** A hyperframe is a placement with
  the `HYPER_FM` flag, so it is written as `{ module: "0x2000", x: 0, y: 0, flags: [HYPER_FM] }`.
  The editor writes the `frame` key for hyperframes, but the game reads `p["module"]`
  and would reject the `frame` form.
* **Flags are names.** `FLIP_X`, `FLIP_Y`, `ROT_90` and `HYPER_FM` are written as a
  list. Bits the editor has no name for, such as the legacy `FREE_ROT`, are kept
  as `extraFlags`.

## What the converter changes

The legacy format and the YAML format disagree in a few places. The converter
resolves them and reports every change it makes.

* **Image IDs are renumbered.** The legacy documents number their images freely
  (`fireball_glow`, `iceball_glow`, `menu` and `shop` start at `0x0001`), the
  modules refer to an image by its *index*. AuroraMX resolves the image of a
  module by index, the game resolves it by *ID*. Numbering the images after their
  index makes both readers agree. `--image-ids keep` keeps the legacy IDs, which
  only works in the editor as long as the IDs match the indexes.
* **Modules without an image get the atlas.** `fireball.sprite` contains the
  module `0x101A` with the image index `-1`. The binary export of the game stores
  no image for it either, so the module is pointed at the first image and a note
  is printed.
* **Image paths use forward slashes.** The legacy documents store Windows paths
  such as `.\effects.tga`, the YAML files use `./effects.tga`.
* **Everything else is kept as it is**: names, collision rectangles, module
  positions, animation times and the order of all lists.

## Publishing the result

The resource build copies every `*.yaml` of `Data/Sprites` into the build and the
game prefers such a file over the binary export of the same sprite
(`Src/Sprites/ASprite.cpp`, `tools/linux/build_resources.py`). A converted file
therefore becomes live as soon as it is placed next to the `*.sprite` file it
belongs to:

```bash
python3 tools/sprites_opencode/convert.py convert Data/Sprites --output Data/Sprites --overwrite
```

`--overwrite` is needed for `Data/Sprites/Ingame/dizzy_effect.yaml`, which was
written by hand; the converted file describes the same sprite.

The image of a module is resolved relative to the sprite file, so the path of an
image keeps pointing at the `.tga` next to it (`./effects.tga`).

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
  allowed in an animation.

The rules of the editor are checked as well: the result is read back with the
same rules as `parseSpriteYaml` of `Tools/AuroraMX/backend/format.ts` and
compared with the source document.

## Verification

```bash
python3 tools/sprites_opencode/test_convert.py
```

The suite covers the legacy reader and writer, the YAML reader and writer, the
game rules and the conversion of every sprite of the project. It checks that all
29 documents of `Data/Sprites` convert, that the result is valid YAML, that it
survives the round trip and that the editor reads it the same way.

The converted documents of the project can be compared with the reference file
`Data/Sprites/Ingame/dizzy_effect.yaml`, which was written by hand in the same
format:

```bash
python3 tools/sprites_opencode/convert.py convert Data/Sprites/Ingame/dizzy_effect.sprite \
    --output build/yaml
diff build/yaml/dizzy_effect.yaml Data/Sprites/Ingame/dizzy_effect.yaml
```

## The files

| File | Contents |
| --- | --- |
| `convert.py` | the command line tool |
| `sprite_model.py` | the document model, the flag tables and the game rules |
| `sprite_text.py` | the reader and the writer of the legacy `.sprite` format |
| `yaml_lite.py` | a small YAML reader for the subset the sprite format uses |
| `sprite_yaml.py` | the reader and the writer of the AuroraMX YAML format |
| `test_convert.py` | the test suite |
