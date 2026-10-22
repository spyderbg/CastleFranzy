"""Document model shared by the legacy .sprite format and the AuroraMX YAML format.

Both formats describe the same sprite: images, ordered modules, ordered frames
with collision rectangles and module placements, and ordered animations with
timed frame steps.  This module holds the neutral representation that the
legacy reader/writer and the YAML reader/writer share.

Two details are worth remembering:

* Module image references are image *indexes* in the legacy format and in
  AuroraMX, but the game resolves the YAML ``image`` field as an image *ID*.
  The converter therefore keeps image IDs aligned with their index so that
  both readers agree (see :func:`normalize_image_ids`).
* Placement flags are a bit mask.  The editor and the game know four of them;
  the legacy format has a fifth (``FREE_ROT``) that only survives as raw bits.
"""

from __future__ import annotations

from dataclasses import dataclass, field

# Flags understood by the AuroraMX editor and by the game's YAML loader.
YAML_FLAGS = {"FLIP_X": 1, "FLIP_Y": 2, "ROT_90": 4, "HYPER_FM": 16}
# The legacy format also knows FREE_ROT, which the game cannot store.
LEGACY_FLAGS = dict(YAML_FLAGS, FREE_ROT=8)

YAML_FLAG_MASK = 0
for _bit in YAML_FLAGS.values():
    YAML_FLAG_MASK |= _bit

# Module parameter names, in the order the legacy format stores them.
MODULE_FIELDS = {
    "MD_IMAGE": ("image", "x", "y", "w", "h"),
    "MD_RECT": ("color", "w", "h"),
    "MD_FILL_RECT": ("color", "w", "h"),
    "MD_MARKER": ("w", "h"),
    "MD_ARC": ("color", "w", "h", "startAngle", "arcAngle"),
    "MD_FILL_ARC": ("color", "w", "h", "startAngle", "arcAngle"),
    "MD_TRIANGLE": ("color", "p2X", "p2Y", "p3X", "p3Y"),
    "MD_FILL_TRIANGLE": ("color", "p2X", "p2Y", "p3X", "p3Y"),
}
# Module fields that are written as hexadecimal strings.
HEX_FIELDS = frozenset({"image", "color"})


class SpriteError(Exception):
    """Raised when a sprite document cannot be read or is inconsistent."""


@dataclass
class SpriteImage:
    id: int
    file: str
    alpha_file: str | None = None
    transparent: int | None = None


@dataclass
class SpriteModule:
    id: int
    type: str
    values: list[int]
    name: str = ""


@dataclass
class Placement:
    target: int
    x: int = 0
    y: int = 0
    flags: int = 0


@dataclass
class SpriteFrame:
    id: int
    name: str = ""
    rects: list[tuple[int, int, int, int]] = field(default_factory=list)
    parts: list[Placement] = field(default_factory=list)


@dataclass
class AnimationStep:
    target: int
    time: int = 0
    x: int = 0
    y: int = 0
    flags: int = 0


@dataclass
class SpriteAnimation:
    id: int
    name: str = ""
    frames: list[AnimationStep] = field(default_factory=list)


@dataclass
class SpriteDocument:
    version: str = "0001"
    images: list[SpriteImage] = field(default_factory=list)
    modules: list[SpriteModule] = field(default_factory=list)
    frames: list[SpriteFrame] = field(default_factory=list)
    animations: list[SpriteAnimation] = field(default_factory=list)
    extras: list[str] = field(default_factory=list)


def hex_id(value: int) -> str:
    """Format an ID the way both readers expect it: a quoted 0x string."""
    return "0x%04X" % value


def hex_color(value: int) -> str:
    """Format a colour as the eight digit hexadecimal string of the legacy format."""
    return "0x%08X" % (value & 0xFFFFFFFF)


def parse_number(text: str) -> int:
    """Read a decimal or hexadecimal integer.

    The legacy format writes plain decimals and ``0x`` values; the YAML
    format adds quoted hexadecimal strings.  Leading zeros stay decimal so
    that values such as ``0001`` are never mistaken for octal.
    """
    body = text.strip()
    if not body:
        raise SpriteError("expected a number, found an empty value")
    sign = 1
    if body[0] in "+-":
        sign = -1 if body[0] == "-" else 1
        body = body[1:].strip()
    try:
        if body[:2].lower() == "0x":
            value = int(body[2:], 16)
        elif body.isdigit():
            value = int(body, 10)
        else:
            raise ValueError(body)
    except ValueError:
        raise SpriteError(f"expected an integer, found {text.strip()!r}") from None
    return sign * value


def flag_names(flags: int) -> list[str]:
    """Return the editor's flag names for the bits that are set."""
    return [name for name, bit in YAML_FLAGS.items() if flags & bit]


def extra_flag_bits(flags: int) -> int:
    """Return the flag bits the editor has no name for (for example FREE_ROT)."""
    return flags & ~YAML_FLAG_MASK


def normalize_image_ids(document: SpriteDocument) -> list[str]:
    """Rewrite image IDs so that they match their index.

    The legacy format stores image IDs that are unrelated to the image index
    modules refer to, and the game resolves YAML image references by ID while
    AuroraMX resolves them by index.  Numbering the images after their index
    makes both readers agree.  The notes describe every change.
    """
    notes = []
    for index, image in enumerate(document.images):
        if image.id != index:
            notes.append(
                "image %s renumbered to %s (index %d)"
                % (hex_id(image.id), hex_id(index), index)
            )
            image.id = index
    return notes


def normalize_image_paths(document: SpriteDocument) -> list[str]:
    """Write the image paths with forward slashes.

    The legacy documents store Windows paths such as ``.\\effects.tga``.  The
    editor and the game both understand forward slashes on every platform, and
    a backslash inside a YAML string would have to be escaped.
    """
    notes = []
    for image in document.images:
        for attribute in ("file", "alpha_file"):
            value = getattr(image, attribute)
            if value and "\\" in value:
                setattr(image, attribute, value.replace("\\", "/"))
                notes.append("image path %s rewritten with forward slashes" % value)
    return notes


def resolve_module_images(document: SpriteDocument) -> list[str]:
    """Point modules that have no image at the atlas.

    The legacy documents mark a module without an image with the index ``-1``.
    The binary export of the game stores no image reference at all, so such a
    module ends up as a rectangle of the atlas.  Naming the atlas keeps the
    converted document loadable and matches what the game draws.
    """
    notes = []
    if not document.images:
        return notes
    for module in document.modules:
        if module.type != "MD_IMAGE" or not module.values:
            continue
        index = module.values[0]
        if 0 <= index < len(document.images):
            continue
        notes.append(
            "module %s has no image; the atlas %s is used instead"
            % (hex_id(module.id), hex_id(document.images[0].id))
        )
        module.values[0] = 0
    return notes


def normalize_document(document: SpriteDocument, image_ids: str = "normalize") -> list[str]:
    """Apply every normalisation a legacy document needs and describe the changes.

    The notes are meant to be shown to the user, they explain what the
    converter had to change to make the document readable by both the editor
    and the game.
    """
    notes = []
    notes.extend(normalize_image_paths(document))
    notes.extend(resolve_module_images(document))
    if image_ids == "normalize":
        notes.extend(normalize_image_ids(document))
    else:
        for index, image in enumerate(document.images):
            if image.id != index:
                notes.append(
                    "image %s keeps its legacy ID; the editor, which resolves images "
                    "by index, will show the wrong picture" % hex_id(image.id)
                )
    return notes


def module_size(module: SpriteModule) -> tuple[int, int]:
    """Return the (width, height) of a module, following the editor's rules."""
    if module.type == "MD_IMAGE":
        return module.values[3], module.values[4]
    if module.type == "MD_MARKER":
        return module.values[0], module.values[1]
    return module.values[1], module.values[2]


def frame_bounds(document: SpriteDocument, frame: SpriteFrame, visiting=()) -> tuple[int, int, int, int]:
    """Compute the bounding box of a frame, transforming hyperframes."""
    if frame.id in visiting:
        raise SpriteError("cyclic hyperframe %s" % hex_id(frame.id))
    seen = tuple(visiting) + (frame.id,)
    boxes = []
    for part in frame.parts:
        if part.flags & YAML_FLAGS["HYPER_FM"]:
            child = next((f for f in document.frames if f.id == part.target), None)
            if child is None:
                raise SpriteError("missing hyperframe %s" % hex_id(part.target))
            x1, y1, x2, y2 = frame_bounds(document, child, seen)
            corners = [(x1, y1), (x2, y1), (x1, y2), (x2, y2)]
            points = []
            for x, y in corners:
                if part.flags & YAML_FLAGS["FLIP_X"]:
                    x = -x
                if part.flags & YAML_FLAGS["FLIP_Y"]:
                    y = -y
                if part.flags & YAML_FLAGS["ROT_90"]:
                    x, y = -y, x
                points.append((x + part.x, y + part.y))
            xs = [p[0] for p in points]
            ys = [p[1] for p in points]
            boxes.append((min(xs), min(ys), max(xs), max(ys)))
        else:
            module = next((m for m in document.modules if m.id == part.target), None)
            if module is None:
                raise SpriteError("missing module %s" % hex_id(part.target))
            width, height = module_size(module)
            if part.flags & YAML_FLAGS["ROT_90"]:
                width, height = height, width
            boxes.append((part.x, part.y, part.x + width, part.y + height))
    if not boxes:
        return (0, 0, 0, 0)
    return (
        min(b[0] for b in boxes),
        min(b[1] for b in boxes),
        max(b[2] for b in boxes),
        max(b[3] for b in boxes),
    )


def validate_game_rules(document: SpriteDocument) -> list[str]:
    """Check the document against the rules of the game's YAML loader.

    The game compiles YAML sprites in ``Src/Sprites/SpriteYaml.cpp``.  This
    mirrors its limits so that a converted document is known to load.
    """
    problems = []
    if document.version != "0001":
        problems.append("the game only loads version \"0001\", found %r" % document.version)
    if not document.images:
        problems.append("a sprite needs at least one image")
    if not document.modules:
        problems.append("a sprite needs at least one module")
    if not document.frames:
        problems.append("a sprite needs at least one frame")
    for label, items, limit in (
        ("image", document.images, 65535),
        ("module", document.modules, 32767),
        ("frame", document.frames, 32767),
        ("animation", document.animations, 32767),
    ):
        if len(items) > limit:
            problems.append("a sprite supports at most %d %ss" % (limit, label))
        seen = set()
        for item in items:
            if item.id in seen:
                problems.append("duplicate %s ID %s" % (label, hex_id(item.id)))
            seen.add(item.id)
            if not 0 <= item.id <= 0xFFFF:
                problems.append(
                    "%s ID %s does not fit into the 16 bits the game uses"
                    % (label, hex_id(item.id))
                )
    for image in document.images:
        if not image.file.strip():
            problems.append("the file name of image %s must not be empty" % hex_id(image.id))
        if image.transparent is not None and not 0 <= image.transparent <= 0xFFFFFFFF:
            problems.append(
                "the transparent colour of image %s is outside 0..0xFFFFFFFF" % hex_id(image.id)
            )
    image_ids = {image.id for image in document.images}
    atlas = None
    for module in document.modules:
        if module.type != "MD_IMAGE":
            problems.append(
                "the game only loads MD_IMAGE modules, found %s in %s"
                % (module.type, hex_id(module.id))
            )
            continue
        if not module.values or module.values[0] not in range(len(document.images)):
            problems.append("%s references a missing image" % hex_id(module.id))
            continue
        image = document.images[module.values[0]]
        if atlas is None:
            atlas = image.id
        elif atlas != image.id:
            problems.append("the game supports one image atlas per sprite")
        for name, value in zip(MODULE_FIELDS["MD_IMAGE"][1:], module.values[1:]):
            if not 0 <= value <= 32767:
                problems.append(
                    "%s has %s=%d outside 0..32767" % (hex_id(module.id), name, value)
                )
    module_ids = {module.id for module in document.modules}
    frame_ids = {frame.id for frame in document.frames}
    frame_indexes = {frame.id: index for index, frame in enumerate(document.frames)}
    part_count = 0
    rect_count = 0
    for frame in document.frames:
        if len(frame.rects) > 255:
            problems.append("%s has more than 255 collision rectangles" % hex_id(frame.id))
        rect_count += len(frame.rects)
        for x1, y1, x2, y2 in frame.rects:
            if x2 < x1 or y2 < y1:
                problems.append("%s has an inverted collision rectangle" % hex_id(frame.id))
            elif not all(-32768 <= value <= 32767 for value in (x1, y1, x2, y2)):
                problems.append("%s has a collision rectangle outside runtime limits" % hex_id(frame.id))
        part_count += len(frame.parts)
        for part in frame.parts:
            if not -32768 <= part.x <= 32767 or not -32768 <= part.y <= 32767:
                problems.append("%s has a placement offset outside runtime limits" % hex_id(frame.id))
            hyper = part.flags & YAML_FLAGS["HYPER_FM"]
            known = frame_ids if hyper else module_ids
            if part.target not in known:
                problems.append(
                    "%s references the unknown %s %s"
                    % (hex_id(frame.id), "frame" if hyper else "module", hex_id(part.target))
                )
        try:
            x1, y1, x2, y2 = frame_bounds(document, frame)
        except SpriteError as error:
            problems.append("%s: %s" % (hex_id(frame.id), error))
        else:
            if not (
                -32768 <= x1 <= 32767
                and -32768 <= y1 <= 32767
                and 0 <= x2 - x1 <= 32767
                and 0 <= y2 - y1 <= 32767
            ):
                problems.append("%s is larger than the runtime allows" % hex_id(frame.id))
    if part_count > 32767:
        problems.append("a sprite supports at most 32767 frame placements")
    if rect_count > 65535:
        problems.append("a sprite supports at most 65535 collision rectangles")
    for animation in document.animations:
        if not animation.frames:
            problems.append("%s needs at least one frame" % hex_id(animation.id))
        for step in animation.frames:
            if step.target not in frame_ids:
                problems.append(
                    "%s references the unknown frame %s"
                    % (hex_id(animation.id), hex_id(step.target))
                )
            elif frame_indexes[step.target] > 1023:
                problems.append(
                    "%s references frame %s beyond the 0..1023 limit"
                    % (hex_id(animation.id), hex_id(step.target))
                )
            if not 0 <= step.time <= 255:
                problems.append("%s has a step time outside 0..255" % hex_id(animation.id))
            if not -128 <= step.x <= 127 or not -128 <= step.y <= 127:
                problems.append("%s has a step offset outside -128..127" % hex_id(animation.id))
            if step.flags & YAML_FLAGS["HYPER_FM"]:
                problems.append(
                    "%s uses the HYPER_FM flag, which the game does not allow in animations"
                    % hex_id(animation.id)
                )
    return problems
