"""A small YAML reader for the subset the sprite format uses.

The sprite format needs block mappings, block sequences, flow mappings, flow
sequences, plain, single and double quoted scalars, block scalars and
comments.  Reading them here keeps the tool free of third party packages;
``test_convert.py`` cross-checks the reader against PyYAML whenever that
package is installed.
"""

from __future__ import annotations

import re

_INT = re.compile(r"^[+-]?(0x[0-9a-fA-F]+|0o[0-7]+|[0-9]+)$")
_FLOAT = re.compile(r"^[+-]?(\.[0-9]+|[0-9]+(\.[0-9]*)?)([eE][+-]?[0-9]+)?$")


class YamlError(Exception):
    """Raised when a YAML document cannot be read."""


def _strip_comment(line: str) -> str:
    """Remove a trailing comment, ignoring ``#`` inside quoted strings."""
    out = []
    quote = None
    index = 0
    while index < len(line):
        char = line[index]
        if quote:
            if quote == '"' and char == "\\":
                out.append(char)
                index += 1
                if index < len(line):
                    out.append(line[index])
                    index += 1
                continue
            out.append(char)
            index += 1
            if char == quote:
                if quote == "'" and index < len(line) and line[index] == "'":
                    out.append(line[index])
                    index += 1
                    continue
                quote = None
            continue
        if char in "\"'":
            quote = char
            out.append(char)
            index += 1
            continue
        if char == "#" and (index == 0 or line[index - 1] in " \t"):
            break
        out.append(char)
        index += 1
    return "".join(out)


def _read_double_quoted(text: str):
    escapes = {
        "n": "\n", "t": "\t", "r": "\r", "0": "\0", "a": "\a", "b": "\b",
        "e": "\x1b", "f": "\f", "v": "\v", "\\": "\\", '"': '"', "/": "/",
        " ": " ", "N": "\x85", "_": "\xa0",
    }
    out = []
    index = 1
    while index < len(text):
        char = text[index]
        if char == "\\":
            index += 1
            if index >= len(text):
                raise YamlError("unterminated escape sequence")
            marker = text[index]
            if marker in escapes:
                out.append(escapes[marker])
                index += 1
            elif marker in "xuU":
                width = {"x": 2, "u": 4, "U": 8}[marker]
                digits = text[index + 1:index + 1 + width]
                if len(digits) != width:
                    raise YamlError("truncated escape sequence")
                try:
                    out.append(chr(int(digits, 16)))
                except ValueError:
                    raise YamlError("invalid escape sequence \\%s%s" % (marker, digits)) from None
                index += 1 + width
            else:
                raise YamlError("unsupported escape sequence \\%s" % marker)
            continue
        if char == '"':
            return "".join(out), index + 1
        out.append(char)
        index += 1
    raise YamlError("unterminated quoted string")


def _read_single_quoted(text: str):
    out = []
    index = 1
    while index < len(text):
        char = text[index]
        if char == "'":
            if index + 1 < len(text) and text[index + 1] == "'":
                out.append("'")
                index += 2
                continue
            return "".join(out), index + 1
        out.append(char)
        index += 1
    raise YamlError("unterminated quoted string")


def _parse_plain(text: str):
    """Resolve a plain scalar to a Python value."""
    value = text.strip()
    if value in ("", "~", "null", "Null", "NULL"):
        return None
    if value in ("true", "True", "TRUE"):
        return True
    if value in ("false", "False", "FALSE"):
        return False
    if _INT.match(value):
        return int(value, 0)
    if _FLOAT.match(value):
        return float(value)
    return value


def _parse_flow_scalar(text: str):
    """Read one scalar of a flow collection."""
    if text.startswith('"'):
        value, index = _read_double_quoted(text)
        return value, text[index:]
    if text.startswith("'"):
        value, index = _read_single_quoted(text)
        return value, text[index:]
    index = 0
    while index < len(text):
        char = text[index]
        if char in ",}]":
            break
        if char == ":" and (index + 1 >= len(text) or text[index + 1] in " \t"):
            break
        index += 1
    return _parse_plain(text[:index]), text[index:]


def _parse_flow_node(text: str):
    text = text.lstrip()
    if not text:
        raise YamlError("unexpected end of flow collection")
    if text[0] == "{":
        return _parse_flow_mapping(text[1:])
    if text[0] == "[":
        return _parse_flow_sequence(text[1:])
    return _parse_flow_scalar(text)


def _parse_flow_mapping(text: str):
    result = {}
    while True:
        text = text.lstrip()
        if not text:
            raise YamlError("unterminated flow mapping")
        if text[0] == "}":
            return result, text[1:]
        key, text = _parse_flow_scalar(text)
        text = text.lstrip()
        if not text.startswith(":"):
            raise YamlError("expected ':' in flow mapping")
        value, text = _parse_flow_node(text[1:])
        result[str(key)] = value
        text = text.lstrip()
        if text.startswith(","):
            text = text[1:]
            continue
        if text.startswith("}"):
            return result, text[1:]
        raise YamlError("expected ',' or '}' in flow mapping")


def _parse_flow_sequence(text: str):
    result = []
    while True:
        text = text.lstrip()
        if not text:
            raise YamlError("unterminated flow sequence")
        if text[0] == "]":
            return result, text[1:]
        value, text = _parse_flow_node(text)
        result.append(value)
        text = text.lstrip()
        if text.startswith(","):
            text = text[1:]
            continue
        if text.startswith("]"):
            return result, text[1:]
        raise YamlError("expected ',' or ']' in flow sequence")


def _split_key(content: str):
    """Split ``key: value`` into its two parts."""
    if content.startswith('"'):
        key, index = _read_double_quoted(content)
        rest = content[index:].lstrip()
        if not rest.startswith(":"):
            raise YamlError("expected ':' after the key %r" % key)
        return key, True, rest[1:]
    if content.startswith("'"):
        key, index = _read_single_quoted(content)
        rest = content[index:].lstrip()
        if not rest.startswith(":"):
            raise YamlError("expected ':' after the key %r" % key)
        return key, True, rest[1:]
    index = content.find(":")
    if index < 0:
        return content, False, ""
    return content[:index].strip(), True, content[index + 1:]


def _fold(lines: list[str]) -> str:
    """Fold the lines of a ``>`` block scalar."""
    folded = []
    for line in lines:
        if not line:
            folded.append("\n")
        elif folded and folded[-1] not in ("\n",):
            folded.append(" " + line)
        else:
            folded.append(line)
    return "".join(folded)


class _Parser:
    def __init__(self, text: str):
        self.lines = text.replace("\r\n", "\n").replace("\r", "\n").split("\n")
        self.pos = 0

    def peek(self):
        """Return the (indent, content) of the next significant line."""
        while self.pos < len(self.lines):
            raw = self.lines[self.pos]
            stripped = raw.strip()
            if not stripped or stripped.startswith("#"):
                self.pos += 1
                continue
            indent = len(raw) - len(raw.lstrip(" "))
            content = _strip_comment(raw).strip()
            if not content:
                self.pos += 1
                continue
            return indent, content
        return None

    def parse_document(self):
        peeked = self.peek()
        if peeked is None:
            return None
        value = self.parse_block(peeked[0])
        if self.peek() is not None:
            raise YamlError("unexpected content after the document")
        return value

    def parse_block(self, indent: int):
        peeked = self.peek()
        if peeked is None:
            return None
        current, content = peeked
        if current < indent:
            return None
        if content == "-" or content.startswith("- "):
            return self.parse_sequence(current)
        return self.parse_mapping(current)

    def parse_mapping(self, indent: int):
        result = {}
        while True:
            peeked = self.peek()
            if peeked is None:
                break
            current, content = peeked
            if current < indent:
                break
            if current > indent:
                raise YamlError("unexpected indentation at %r" % content)
            key, separator, rest = _split_key(content)
            if not separator:
                raise YamlError("expected 'key: value', found %r" % content)
            self.pos += 1
            rest = rest.strip()
            if not rest:
                peeked = self.peek()
                if peeked is not None and peeked[0] == indent and (
                    peeked[1] == "-" or peeked[1].startswith("- ")
                ):
                    value = self.parse_sequence(indent)
                else:
                    value = self.parse_block(indent + 1)
            elif rest[0] in "|>":
                value = self.parse_block_scalar(rest, indent)
            else:
                value = self.parse_inline(rest)
            if key in result:
                raise YamlError("duplicate key %r" % key)
            result[key] = value
        return result

    def parse_sequence(self, indent: int):
        result = []
        while True:
            peeked = self.peek()
            if peeked is None:
                break
            current, content = peeked
            if current < indent:
                break
            if current > indent:
                raise YamlError("unexpected indentation at %r" % content)
            if not (content == "-" or content.startswith("- ")):
                break
            rest = content[1:]
            offset = len(rest) - len(rest.lstrip(" "))
            rest = rest.strip()
            item_indent = current + 1 + offset
            if not rest:
                self.pos += 1
                result.append(self.parse_block(indent + 1))
                continue
            if rest[0] in "{[":
                self.pos += 1
                result.append(self.parse_inline(rest))
                continue
            _, separator, _ = _split_key(rest)
            if separator:
                # A mapping that starts on the dash line: rewrite the line so
                # that the mapping parser sees it at its real column.
                self.lines[self.pos] = " " * item_indent + rest
                result.append(self.parse_mapping(item_indent))
                continue
            self.pos += 1
            result.append(self.parse_inline(rest))
        return result

    def parse_inline(self, text: str):
        text = text.strip()
        if not text:
            return None
        if text[0] in "{[":
            value, rest = _parse_flow_node(text)
            if rest.strip():
                raise YamlError("unexpected content after a flow collection")
            return value
        if text.startswith('"'):
            value, index = _read_double_quoted(text)
            if text[index:].strip():
                raise YamlError("unexpected content after a quoted scalar")
            return value
        if text.startswith("'"):
            value, index = _read_single_quoted(text)
            if text[index:].strip():
                raise YamlError("unexpected content after a quoted scalar")
            return value
        return _parse_plain(text)

    def parse_block_scalar(self, style: str, indent: int):
        chomping = ""
        if style[-1] in "+-":
            chomping = style[-1]
            style = style[:-1]
        lines = []
        block_indent = None
        while self.pos < len(self.lines):
            raw = self.lines[self.pos]
            if not raw.strip():
                lines.append("")
                self.pos += 1
                continue
            current = len(raw) - len(raw.lstrip(" "))
            if current <= indent:
                break
            if block_indent is None:
                block_indent = current
            if current < block_indent:
                break
            lines.append(raw[block_indent:])
            self.pos += 1
        trailing = 0
        while lines and not lines[-1].strip():
            lines.pop()
            trailing += 1
        text = _fold(lines) if style == ">" else "\n".join(lines)
        if chomping == "-":
            return text
        if chomping == "+":
            return text + "\n" * (trailing + 1) if text else "\n" * trailing
        return text + "\n" if text else ""


def parse(text: str):
    """Read a YAML document and return the plain Python data it describes."""
    return _Parser(text).parse_document()
