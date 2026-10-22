"""Reader and writer for the AuroraMX YAML sprite format.

The document is a mapping under a top level ``sprite`` key::

    sprite:
      version: "0001"
      images:
        - { id: "0x0000", file: "./effects.tga", transparent: "0x00FF00FF" }
      modules:
        - { id: "0x1000", type: MD_IMAGE, image: "0x0000", x: 0, y: 0, w: 30, h: 26 }
      frames:
        - id: "0x2000"
          name: ""
          parts:
            - { module: "0x1000", x: -15, y: -13 }
      animations:
        - id: "0x3000"
          name: ""
          frames:
            - { frame: "0x2000", time: 2, x: 0, y: 0 }

IDs are quoted hexadecimal strings, because both the game's YAML loader and the
editor resolve them by their text form.  A frame placement always uses the
``module`` key; a hyperframe placement adds the ``HYPER_FM`` flag.  The editor
also accepts a ``frame`` key for hyperframes, but the game does not.
"""

from __future__ import annotations

import yaml_lite

from sprite_model import (
    HEX_FIELDS,
    MODULE_FIELDS,
    YAML_FLAGS,
    AnimationStep,
    Placement,
    SpriteAnimation,
    SpriteDocument,
    SpriteError,
    SpriteFrame,
    SpriteImage,
    SpriteModule,
    extra_flag_bits,
    flag_names,
    hex_color,
    hex_id,
)


def _quote(value: str) -> str:
    escaped = (
        value.replace("\\", "\\\\")
        .replace('"', '\\"')
        .replace("\n", "\\n")
        .replace("\r", "\\r")
        .replace("\t", "\\t")
    )
    return '"%s"' % escaped


def _placement(target_key: str, target: int, x: int, y: int, flags: int, items=None) -> str:
    """Write a placement as a flow mapping."""
    entries = ["%s: \"%s\"" % (target_key, hex_id(target))]
    entries.extend(items or [])
    entries.append("x: %d" % x)
    entries.append("y: %d" % y)
    names = flag_names(flags)
    if names:
        entries.append("flags: [%s]" % ", ".join(names))
    extra = extra_flag_bits(flags)
    if extra:
        entries.append("extraFlags: %d" % extra)
    return "{ " + ", ".join(entries) + " }"


def _image_reference(document, index: int) -> int:
    """Return the ID of the image a module points at.

    The legacy format and the editor refer to an image by its *index*, the
    game resolves the YAML reference by *ID*.  Both agree when the images are
    numbered after their index, which is what the converter does by default.
    """
    if 0 <= index < len(document.images):
        return document.images[index].id
    return index


def dump_yaml(document: SpriteDocument, header: list[str] | None = None) -> str:
    """Write a document in the AuroraMX YAML format."""
    lines = []
    for comment in header or []:
        lines.append("# " + comment if comment else "#")
    lines.append("sprite:")
    lines.append("  version: %s" % _quote(document.version))

    lines.append("")
    if document.images:
        lines.append("  images:")
        for image in document.images:
            lines.append("    - id: \"%s\"" % hex_id(image.id))
            lines.append("      file: %s" % _quote(image.file))
            if image.alpha_file:
                lines.append("      alphaFile: %s" % _quote(image.alpha_file))
            if image.transparent is not None:
                lines.append("      transparent: \"%s\"" % hex_color(image.transparent))
    else:
        lines.append("  images: []")

    lines.append("")
    if document.modules:
        lines.append("  modules:")
        for module in document.modules:
            fields = MODULE_FIELDS.get(module.type)
            if fields is None:
                raise SpriteError("unknown module type %s" % module.type)
            lines.append("    - id: \"%s\"" % hex_id(module.id))
            lines.append("      type: %s" % module.type)
            for name, value in zip(fields, module.values):
                if name == "image":
                    value = _image_reference(document, value)
                if name in HEX_FIELDS:
                    lines.append("      %s: \"%s\"" % (name, hex_id(value)))
                else:
                    lines.append("      %s: %d" % (name, value))
            if module.name:
                lines.append("      name: %s" % _quote(module.name))
    else:
        lines.append("  modules: []")

    lines.append("")
    if document.frames:
        lines.append("  frames:")
        for frame in document.frames:
            lines.append("    - id: \"%s\"" % hex_id(frame.id))
            lines.append("      name: %s" % _quote(frame.name))
            if frame.rects:
                lines.append("      rects:")
                for x1, y1, x2, y2 in frame.rects:
                    lines.append(
                        "        - { x1: %d, y1: %d, x2: %d, y2: %d }" % (x1, y1, x2, y2)
                    )
            lines.append("      parts:")
            for part in frame.parts:
                lines.append("        - " + _placement("module", part.target, part.x, part.y, part.flags))
    else:
        lines.append("  frames: []")

    lines.append("")
    if document.animations:
        lines.append("  animations:")
        for animation in document.animations:
            lines.append("    - id: \"%s\"" % hex_id(animation.id))
            lines.append("      name: %s" % _quote(animation.name))
            lines.append("      frames:")
            for step in animation.frames:
                lines.append(
                    "        - "
                    + _placement(
                        "frame",
                        step.target,
                        step.x,
                        step.y,
                        step.flags,
                        ["time: %d" % step.time],
                    )
                )
    else:
        lines.append("  animations: []")

    if document.extras:
        lines.append("")
        lines.append("  extensions:")
        for extra in document.extras:
            lines.append("    - |")
            for line in extra.splitlines() or [""]:
                lines.append("      " + line if line else "")

    lines.append("")
    return "\n".join(lines)


def _mapping(value, what: str) -> dict:
    if not isinstance(value, dict):
        raise SpriteError("%s must be a mapping" % what)
    return value


def _sequence(value, what: str) -> list:
    if value is None:
        return []
    if not isinstance(value, list):
        raise SpriteError("%s must be a list" % what)
    return value


def _number(value, what: str) -> int:
    if isinstance(value, bool):
        raise SpriteError("%s must be an integer" % what)
    if isinstance(value, int):
        return value
    if isinstance(value, float) and value.is_integer():
        return int(value)
    if isinstance(value, str):
        from sprite_model import parse_number

        try:
            return parse_number(value)
        except SpriteError:
            raise SpriteError("%s must be an integer, found %r" % (what, value)) from None
    raise SpriteError("%s must be an integer, found %r" % (what, value))


def _identifier(value, what: str) -> int:
    """Read an ID: a quoted hexadecimal string or a plain integer."""
    if isinstance(value, str):
        from sprite_model import parse_number

        try:
            return parse_number(value)
        except SpriteError:
            raise SpriteError("%s must be a hexadecimal ID, found %r" % (what, value)) from None
    return _number(value, what)


def _flags(entry: dict, what: str) -> int:
    flags = 0
    for name in _sequence(entry.get("flags"), "%s.flags" % what):
        if name not in YAML_FLAGS:
            raise SpriteError("%s: unknown flag %s" % (what, name))
        flags |= YAML_FLAGS[name]
    if entry.get("extraFlags") is not None:
        flags |= _number(entry["extraFlags"], "%s.extraFlags" % what)
    return flags


def parse_yaml(text: str) -> SpriteDocument:
    """Read an AuroraMX YAML sprite document."""
    data = yaml_lite.parse(text)
    if not isinstance(data, dict):
        raise SpriteError("YAML sprite files need a top-level 'sprite' mapping")
    sprite = data["sprite"] if "sprite" in data else data
    sprite = _mapping(sprite, "sprite")

    document = SpriteDocument()
    if sprite.get("version") is not None:
        document.version = str(sprite["version"])

    for index, entry in enumerate(_sequence(sprite.get("images"), "images")):
        entry = _mapping(entry, "images[%d]" % index)
        image = SpriteImage(
            id=_identifier(entry.get("id"), "images[%d].id" % index),
            file=str(entry.get("file", "")),
        )
        if entry.get("alphaFile") is not None:
            image.alpha_file = str(entry["alphaFile"])
        if entry.get("transparent") is not None:
            image.transparent = _number(entry["transparent"], "images[%d].transparent" % index)
        document.images.append(image)

    image_indexes = {image.id: index for index, image in enumerate(document.images)}
    for index, entry in enumerate(_sequence(sprite.get("modules"), "modules")):
        entry = _mapping(entry, "modules[%d]" % index)
        type_name = str(entry.get("type", ""))
        fields = MODULE_FIELDS.get(type_name)
        if fields is None:
            raise SpriteError("unknown module type %s" % type_name)
        values = []
        for name in fields:
            what = "modules[%d].%s" % (index, name)
            if name not in entry:
                raise SpriteError("missing %s" % what)
            value = entry[name]
            if name == "image" and isinstance(value, str):
                # The game resolves image references by ID.
                identifier = _identifier(value, what)
                if identifier not in image_indexes:
                    raise SpriteError("%s references the unknown image %s" % (what, hex_id(identifier)))
                values.append(image_indexes[identifier])
            elif name == "image":
                values.append(_number(value, what))
            else:
                values.append(_number(value, what))
        name = entry.get("name")
        document.modules.append(
            SpriteModule(
                id=_identifier(entry.get("id"), "modules[%d].id" % index),
                type=type_name,
                values=values,
                name="" if name is None else str(name),
            )
        )

    for index, entry in enumerate(_sequence(sprite.get("frames"), "frames")):
        entry = _mapping(entry, "frames[%d]" % index)
        frame = SpriteFrame(
            id=_identifier(entry.get("id"), "frames[%d].id" % index),
            name="" if entry.get("name") is None else str(entry["name"]),
        )
        for rect in _sequence(entry.get("rects"), "frames[%d].rects" % index):
            rect = _mapping(rect, "frames[%d].rects" % index)
            frame.rects.append(
                (
                    _number(rect.get("x1"), "rect x1"),
                    _number(rect.get("y1"), "rect y1"),
                    _number(rect.get("x2"), "rect x2"),
                    _number(rect.get("y2"), "rect y2"),
                )
            )
        for part in _sequence(entry.get("parts"), "frames[%d].parts" % index):
            part = _mapping(part, "frames[%d].parts" % index)
            what = "frames[%d].parts" % index
            if "module" in part:
                target = _identifier(part["module"], what)
            elif "frame" in part:
                target = _identifier(part["frame"], what)
            else:
                raise SpriteError("%s needs a module or frame target" % what)
            flags = _flags(part, what)
            if "module" not in part:
                flags |= YAML_FLAGS["HYPER_FM"]
            frame.parts.append(
                Placement(
                    target=target,
                    x=_number(part.get("x", 0), what + ".x"),
                    y=_number(part.get("y", 0), what + ".y"),
                    flags=flags,
                )
            )
        document.frames.append(frame)

    for index, entry in enumerate(_sequence(sprite.get("animations"), "animations")):
        entry = _mapping(entry, "animations[%d]" % index)
        animation = SpriteAnimation(
            id=_identifier(entry.get("id"), "animations[%d].id" % index),
            name="" if entry.get("name") is None else str(entry["name"]),
        )
        for step in _sequence(entry.get("frames"), "animations[%d].frames" % index):
            step = _mapping(step, "animations[%d].frames" % index)
            what = "animations[%d].frames" % index
            if "frame" not in step:
                raise SpriteError("%s needs a frame target" % what)
            animation.frames.append(
                AnimationStep(
                    target=_identifier(step["frame"], what),
                    time=_number(step.get("time", 0), what + ".time"),
                    x=_number(step.get("x", 0), what + ".x"),
                    y=_number(step.get("y", 0), what + ".y"),
                    flags=_flags(step, what),
                )
            )
        document.animations.append(animation)

    for index, extra in enumerate(_sequence(sprite.get("extensions"), "extensions")):
        document.extras.append(str(extra))

    return document
