"""Reader and writer for the legacy AuroraGT ``.sprite`` text format.

The format is a small block language::

    /*SPRITE*/ {
        VERSION 0001
        IMAGE 0x0000 ".\\effects.tga" [ALPHA "mask.tga"] [TRANSP 0x00FF00FF]
        MODULES { MD 0x1000 MD_IMAGE 0 3 5 7 7 ["name"] }
        FRAME ["name"] { 0x2000 RC x1 y1 x2 y2 FM 0x1000 -13 7 [FLAGS 0x20] [+FLIP_X] }
        ANIM ["name"] { 0x3000 AF 0x2000 2 0 0 }
        SPRITE_END
    }

Comments start with ``//`` or are written as ``/* ... */`` blocks.  Numbers
are decimal or hexadecimal, and file names are quoted strings.
"""

from __future__ import annotations

import re

from sprite_model import (
    LEGACY_FLAGS,
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
    hex_id,
)

_TOKEN = re.compile(
    r"//[^\r\n]*"                          # line comment
    r"|/\*[\s\S]*?\*/"                     # block comment
    r"|\"(?:\\.|[^\"\\\r\n])*\""           # quoted string
    r"|[{}]"                               # braces
    r"|[^\s{}\"]+",                        # word
    re.ASCII,
)


class _Token:
    __slots__ = ("value", "line", "start", "end", "quoted")

    def __init__(self, value, line, start, end, quoted):
        self.value = value
        self.line = line
        self.start = start
        self.end = end
        self.quoted = quoted

    def __repr__(self):  # pragma: no cover - debugging helper
        return "Token(%r, line=%d)" % (self.value, self.line)


def decode_text(data: bytes) -> str:
    """Decode sprite text, accepting the encodings the editor supports.

    Decoding is strict: a file that is none of them is reported as an error
    instead of being guessed at, because a guessed file name would be written
    back into the converted document.
    """
    try:
        if data[:2] == b"\xff\xfe":
            return data[2:].decode("utf-16-le")
        if data[:2] == b"\xfe\xff":
            return data[2:].decode("utf-16-be")
        try:
            return data.decode("utf-8")
        except UnicodeDecodeError:
            return data.decode("windows-1252")
    except UnicodeDecodeError as error:
        raise SpriteError("the text is neither UTF-8, UTF-16 nor Windows-1252: %s" % error) from None


def _tokenize(text: str) -> list[_Token]:
    tokens = []
    line = 1
    last = 0
    for match in _TOKEN.finditer(text):
        line += text.count("\n", last, match.start())
        last = match.start()
        raw = match.group(0)
        if raw.startswith("//") or raw.startswith("/*"):
            continue
        quoted = raw.startswith('"')
        value = raw[1:-1].replace('\\"', '"') if quoted else raw
        tokens.append(_Token(value, line, match.start(), match.end(), quoted))
    return tokens


class _Reader:
    def __init__(self, text: str):
        self.text = text
        self.tokens = _tokenize(text)
        self.index = 0

    # -- token helpers -------------------------------------------------
    def peek(self):
        return self.tokens[self.index] if self.index < len(self.tokens) else None

    def peek_value(self):
        token = self.peek()
        return token.value if token else None

    def take(self):
        token = self.peek()
        if token is None:
            raise SpriteError("unexpected end of .sprite file")
        self.index += 1
        return token

    def expect(self, value: str):
        token = self.take()
        if token.value != value:
            raise SpriteError(
                "line %d: expected %s, found %s" % (token.line, value, token.value)
            )
        return token

    def number(self) -> int:
        token = self.take()
        try:
            return int(token.value, 0) if token.value[:2].lower() == "0x" else int(token.value)
        except ValueError:
            raise SpriteError(
                "line %d: expected an integer, found %s" % (token.line, token.value)
            ) from None

    def quoted(self, what: str) -> str:
        token = self.take()
        if not token.quoted:
            raise SpriteError(
                "line %d: expected a quoted %s, found %s" % (token.line, what, token.value)
            )
        return token.value

    def optional_quoted(self) -> str:
        token = self.peek()
        if token is not None and token.quoted:
            self.index += 1
            return token.value
        return ""

    def placement(self, count: int):
        """Read a placement: count values plus the flags that follow them."""
        token = self.peek()
        if token is None:
            raise SpriteError("unexpected end of .sprite file")
        line = token.line
        values = [self.number() for _ in range(count)]
        flags = 0
        while True:
            token = self.peek()
            if token is None or token.line != line or token.value == "}":
                break
            self.index += 1
            if token.value == "FLAGS":
                flags |= self.number()
            elif token.value.startswith("+") and token.value[1:] in LEGACY_FLAGS:
                flags |= LEGACY_FLAGS[token.value[1:]]
            else:
                raise SpriteError(
                    "line %d: unsupported placement flag %s" % (token.line, token.value)
                )
        return values, flags


def parse_sprite(text: str) -> SpriteDocument:
    """Parse a legacy ``.sprite`` document."""
    text = text.replace("\r\n", "\n").replace("\r", "\n")
    reader = _Reader(text)
    document = SpriteDocument()

    if reader.peek_value() == "SPRITE":
        reader.index += 1
    reader.expect("{")
    while reader.peek_value() != "}":
        if reader.peek() is None:
            raise SpriteError("unexpected end of .sprite file")
        keyword = reader.take()
        if keyword.value == "VERSION":
            document.version = reader.take().value
        elif keyword.value == "IMAGE":
            identifier = reader.number()
            image = SpriteImage(id=identifier, file=reader.quoted("image file name"))
            while reader.peek_value() in ("ALPHA", "TRANSP"):
                option = reader.take().value
                if option == "ALPHA":
                    image.alpha_file = reader.quoted("alpha mask file name")
                else:
                    image.transparent = reader.number()
            document.images.append(image)
        elif keyword.value == "MODULES":
            reader.expect("{")
            while reader.peek_value() != "}":
                reader.expect("MD")
                identifier = reader.number()
                type_name = reader.take().value
                fields = MODULE_FIELDS.get(type_name)
                if fields is None:
                    raise SpriteError(
                        "line %d: unknown module type %s" % (keyword.line, type_name)
                    )
                values = [reader.number() for _ in fields]
                name = reader.optional_quoted()
                document.modules.append(
                    SpriteModule(id=identifier, type=type_name, values=values, name=name)
                )
            reader.expect("}")
        elif keyword.value == "FRAME":
            name = reader.optional_quoted()
            reader.expect("{")
            identifier = reader.number()
            frame = SpriteFrame(id=identifier, name=name)
            while reader.peek_value() != "}":
                statement = reader.take()
                if statement.value == "RC":
                    x1, y1, x2, y2 = [reader.number() for _ in range(4)]
                    frame.rects.append((x1, y1, x2, y2))
                elif statement.value == "FM":
                    (target, x, y), flags = reader.placement(3)
                    frame.parts.append(Placement(target=target, x=x, y=y, flags=flags))
                else:
                    raise SpriteError(
                        "line %d: unknown frame statement %s"
                        % (statement.line, statement.value)
                    )
            reader.expect("}")
            document.frames.append(frame)
        elif keyword.value == "ANIM":
            name = reader.optional_quoted()
            reader.expect("{")
            identifier = reader.number()
            animation = SpriteAnimation(id=identifier, name=name)
            while reader.peek_value() != "}":
                reader.expect("AF")
                (target, time, x, y), flags = reader.placement(4)
                animation.frames.append(
                    AnimationStep(target=target, time=time, x=x, y=y, flags=flags)
                )
            reader.expect("}")
            document.animations.append(animation)
        elif keyword.value == "SPRITE_END":
            continue
        else:
            # Unknown editor extensions are kept verbatim so that saving the
            # document does not lose them.
            end = keyword.end
            while reader.peek() is not None and reader.peek().line == keyword.line \
                    and reader.peek_value() not in ("{", "}"):
                end = reader.take().end
            if reader.peek_value() == "{":
                depth = 0
                while True:
                    token = reader.take()
                    if token.value == "{":
                        depth += 1
                    elif token.value == "}":
                        depth -= 1
                    end = token.end
                    if depth == 0:
                        break
            document.extras.append(text[keyword.start:end])
    reader.expect("}")
    if reader.index != len(reader.tokens):
        token = reader.tokens[reader.index]
        raise SpriteError(
            "line %d: unexpected content after the sprite" % token.line
        )
    return document


def _quote(value: str) -> str:
    """Write a quoted string.

    The legacy format uses the backslash as a path separator and does not
    escape it, so only the quote character itself is escaped.
    """
    return '"' + value.replace('"', '\\"') + '"'


def _flags(flags: int) -> str:
    """Write the flag bits as names, plus the raw rest.

    Only the four flags that the editor and the game know are written by name;
    every other bit, the legacy ``FREE_ROT`` for example, is written as a raw
    value because no reader knows a name for it.
    """
    parts = []
    rest = flags
    for name, bit in YAML_FLAGS.items():
        if flags & bit:
            parts.append("+" + name)
            rest &= ~bit
    if rest:
        parts.append("FLAGS 0x%02X" % rest)
    return (" " + " ".join(parts)) if parts else ""


def dump_sprite(document: SpriteDocument, header: str | None = None) -> str:
    """Write a legacy ``.sprite`` document."""
    lines = []
    if header:
        lines.extend("// " + line for line in header.splitlines())
    lines.append("/*SPRITE*/ {")
    lines.append("    VERSION %s" % document.version)
    for image in document.images:
        line = "    IMAGE %s %s" % (hex_id(image.id), _quote(image.file))
        if image.alpha_file:
            line += " ALPHA %s" % _quote(image.alpha_file)
        if image.transparent is not None:
            line += " TRANSP 0x%08X" % (image.transparent & 0xFFFFFFFF)
        lines.append(line)
    lines.append("")
    lines.append("    MODULES")
    lines.append("    {")
    for module in document.modules:
        line = "        MD %s %s %s" % (
            hex_id(module.id), module.type, " ".join(str(v) for v in module.values)
        )
        if module.name:
            line += " " + _quote(module.name)
        lines.append(line)
    lines.append("    }")
    lines.append("")
    for frame in document.frames:
        lines.append("    FRAME %s" % _quote(frame.name))
        lines.append("    {")
        lines.append("        %s" % hex_id(frame.id))
        for x1, y1, x2, y2 in frame.rects:
            lines.append("        RC %d %d %d %d" % (x1, y1, x2, y2))
        for part in frame.parts:
            lines.append(
                "        FM %s %d %d%s"
                % (hex_id(part.target), part.x, part.y, _flags(part.flags))
            )
        lines.append("    }")
        lines.append("")
    for animation in document.animations:
        lines.append("    ANIM %s" % _quote(animation.name))
        lines.append("    {")
        lines.append("        %s" % hex_id(animation.id))
        for step in animation.frames:
            lines.append(
                "        AF %s %d %d %d%s"
                % (hex_id(step.target), step.time, step.x, step.y, _flags(step.flags))
            )
        lines.append("    }")
        lines.append("")
    for extra in document.extras:
        lines.extend(extra.splitlines())
    lines.append("    SPRITE_END")
    lines.append("}")
    lines.append("")
    return "\n".join(lines)
