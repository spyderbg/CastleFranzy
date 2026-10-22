#!/usr/bin/env python3
"""Convert legacy AuroraGT ``.sprite`` documents into the AuroraMX ``.yaml`` format.

The editor in ``Tools/AuroraMX`` reads the legacy ``.sprite`` documents of the
game and writes them as YAML.  The YAML files are also what the game loads
directly (``Src/Sprites/SpriteYaml.cpp``), so a converted document has to
satisfy both readers.

Usage::

    python3 tools/sprites_opencode/convert.py convert Data/Sprites/Ingame/effects.sprite
    python3 tools/sprites_opencode/convert.py convert Data/Sprites --output build/yaml
    python3 tools/sprites_opencode/convert.py reverse build/yaml/Ingame/effects.yaml
    python3 tools/sprites_opencode/convert.py verify Data/Sprites/Menu/menu.sprite
    python3 tools/sprites_opencode/convert.py info Data/Sprites/Menu/menu.sprite

Every conversion is checked: the written YAML is read back and compared with
the source document, and the result is validated against the rules of the
game's YAML loader.
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from sprite_model import (  # noqa: E402  (the path has to be set up first)
    SpriteDocument,
    SpriteError,
    hex_id,
    normalize_document,
    validate_game_rules,
)
from sprite_text import decode_text, dump_sprite, parse_sprite  # noqa: E402
from sprite_yaml import dump_yaml, parse_yaml  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
AURORAMX_CLI = ROOT / "Tools" / "AuroraMX" / "build" / "cli.cjs"


def collect(inputs, pattern: str) -> list[Path]:
    """Expand the command line inputs into the list of files to convert."""
    files = []
    for item in inputs:
        path = Path(item)
        if path.is_dir():
            files.extend(sorted(p for p in path.rglob(pattern) if p.is_file()))
        elif path.is_file():
            files.append(path)
        else:
            raise SpriteError("no such file or directory: %s" % item)
    return files


def target_path(source: Path, output: Path | None, suffix: str, root: Path | None) -> Path:
    """Work out where the result of a conversion belongs."""
    if output is None:
        return source.with_suffix(suffix)
    if root is not None:
        return output / source.relative_to(root).with_suffix(suffix)
    return output / source.with_suffix(suffix).name


def read_document(path: Path) -> SpriteDocument:
    """Read a ``.sprite`` or a ``.yaml`` document."""
    text = decode_text(path.read_bytes())
    if path.suffix.lower() in (".yaml", ".yml"):
        return parse_yaml(text)
    return parse_sprite(text)


def describe(document: SpriteDocument) -> str:
    return "%d image%s, %d module%s, %d frame%s, %d animation%s" % (
        len(document.images),
        "" if len(document.images) == 1 else "s",
        len(document.modules),
        "" if len(document.modules) == 1 else "s",
        len(document.frames),
        "" if len(document.frames) == 1 else "s",
        len(document.animations),
        "" if len(document.animations) == 1 else "s",
    )


def check_with_auroramx(path: Path) -> str | None:
    """Ask the AuroraMX command line tool to load the file, when it is available."""
    if not AURORAMX_CLI.is_file():
        return "the AuroraMX command line tool is not built (%s)" % AURORAMX_CLI
    bundle = AURORAMX_CLI.read_text(errors="replace")
    if "top-level 'sprite' mapping" not in bundle:
        return (
            "the built AuroraMX command line tool cannot read YAML yet, "
            "rebuild it with 'npm run build:cli' in Tools/AuroraMX"
        )
    try:
        finished = subprocess.run(
            ["node", str(AURORAMX_CLI), "validate", str(path)],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )
    except OSError as error:
        return "could not run node: %s" % error
    output = finished.stdout.strip()
    if finished.returncode:
        return "rejected: %s" % (output.splitlines()[-1] if output else "unknown error")
    return None


def convert_document(source: Path, target: Path, options) -> list[str]:
    """Convert one legacy document and write the YAML result."""
    document = parse_sprite(decode_text(source.read_bytes()))
    notes = normalize_document(document, options.image_ids)
    header = [
        "Converted from %s by tools/sprites_opencode." % source.name,
        "Edit with AuroraMX (Tools/AuroraMX); the game reads this file directly.",
    ]
    text = dump_yaml(document, header=header)
    if options.verify:
        restored = parse_yaml(text)
        if restored != document:
            raise SpriteError("%s: the converted document does not match the source" % source)
    for problem in validate_game_rules(document):
        notes.append("warning: " + problem)
    if target.exists() and not options.overwrite:
        raise SpriteError("%s exists (use --overwrite to replace it)" % target)
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_text(text, encoding="utf-8", newline="\n")
    if options.auroramx:
        problem = check_with_auroramx(target)
        if problem:
            notes.append("warning: AuroraMX %s" % problem)
    return notes


def reverse_document(source: Path, target: Path, options) -> list[str]:
    """Convert one YAML document back into the legacy format."""
    document = parse_yaml(decode_text(source.read_bytes()))
    text = dump_sprite(
        document,
        header="Restored from %s by tools/sprites_opencode." % source.name,
    )
    if options.verify:
        restored = parse_sprite(text)
        if restored != document:
            raise SpriteError("%s: the converted document does not match the source" % source)
    if target.exists() and not options.overwrite:
        raise SpriteError("%s exists (use --overwrite to replace it)" % target)
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_text(text, encoding="utf-8", newline="\n")
    return []


def command_convert(options) -> int:
    """Convert legacy documents into YAML."""
    failures = 0
    for item in options.inputs:
        path = Path(item)
        root = path if path.is_dir() else path.parent
        for source in collect([item], "*.sprite"):
            target = target_path(source, options.output, ".yaml", root)
            try:
                notes = convert_document(source, target, options)
            except SpriteError as error:
                print("%s: %s" % (source, error), file=sys.stderr)
                failures += 1
                continue
            if not options.quiet:
                print("%s -> %s  (%s)" % (source, target, describe(read_document(target))))
                for note in notes:
                    print("  %s" % note)
    return 1 if failures else 0


def command_reverse(options) -> int:
    """Convert YAML documents back into the legacy format."""
    failures = 0
    for item in options.inputs:
        path = Path(item)
        root = path if path.is_dir() else path.parent
        for source in collect([item], "*.yaml"):
            target = target_path(source, options.output, ".sprite", root)
            try:
                reverse_document(source, target, options)
            except SpriteError as error:
                print("%s: %s" % (source, error), file=sys.stderr)
                failures += 1
                continue
            if not options.quiet:
                print("%s -> %s" % (source, target))
    return 1 if failures else 0


def command_verify(options) -> int:
    """Read documents and report what they contain and what is wrong with them."""
    failures = 0
    for item in options.inputs:
        for source in collect([item], "*"):
            if source.suffix.lower() not in (".sprite", ".yaml", ".yml"):
                continue
            try:
                document = read_document(source)
            except SpriteError as error:
                print("%s: %s" % (source, error), file=sys.stderr)
                failures += 1
                continue
            problems = validate_game_rules(document)
            if not options.quiet:
                print("%s: %s" % (source, describe(document)))
            for problem in problems:
                print("%s: warning: %s" % (source, problem))
                failures += 1
            if options.auroramx:
                problem = check_with_auroramx(source)
                if problem:
                    print("%s: warning: AuroraMX %s" % (source, problem))
                    failures += 1
    return 1 if failures else 0


def command_info(options) -> int:
    """Print a summary of every document."""
    for item in options.inputs:
        for source in collect([item], "*"):
            if source.suffix.lower() not in (".sprite", ".yaml", ".yml"):
                continue
            document = read_document(source)
            print("%s: %s" % (source, describe(document)))
            print("  version %s" % document.version)
            for image in document.images:
                print("  image %s %s" % (hex_id(image.id), image.file))
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="sprites_opencode",
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    commands = parser.add_subparsers(dest="command", required=True)

    convert = commands.add_parser(
        "convert", help="convert legacy .sprite documents into .yaml"
    )
    convert.add_argument("inputs", nargs="+", help="files or directories to convert")
    convert.add_argument(
        "-o", "--output", type=Path, help="write the results into this directory"
    )
    convert.add_argument(
        "--overwrite", action="store_true", help="replace files that already exist"
    )
    convert.add_argument(
        "--image-ids",
        choices=("normalize", "keep"),
        default="normalize",
        help="number the images after their index (default) or keep the legacy IDs",
    )
    convert.add_argument(
        "--no-verify",
        dest="verify",
        action="store_false",
        help="skip the read back check of the written file",
    )
    convert.add_argument(
        "--auroramx",
        action="store_true",
        help="load the result with the AuroraMX command line tool as well",
    )
    convert.add_argument("-q", "--quiet", action="store_true", help="only report problems")
    convert.set_defaults(handler=command_convert, verify=True)

    reverse = commands.add_parser(
        "reverse", help="convert .yaml documents back into .sprite"
    )
    reverse.add_argument("inputs", nargs="+", help="files or directories to convert")
    reverse.add_argument(
        "-o", "--output", type=Path, help="write the results into this directory"
    )
    reverse.add_argument(
        "--overwrite", action="store_true", help="replace files that already exist"
    )
    reverse.add_argument(
        "--no-verify",
        dest="verify",
        action="store_false",
        help="skip the read back check of the written file",
    )
    reverse.add_argument("-q", "--quiet", action="store_true", help="only report problems")
    reverse.set_defaults(handler=command_reverse, verify=True)

    verify = commands.add_parser("verify", help="read documents and report problems")
    verify.add_argument("inputs", nargs="+", help="files or directories to check")
    verify.add_argument(
        "--auroramx",
        action="store_true",
        help="load the documents with the AuroraMX command line tool as well",
    )
    verify.add_argument("-q", "--quiet", action="store_true", help="only report problems")
    verify.set_defaults(handler=command_verify)

    info = commands.add_parser("info", help="print a summary of every document")
    info.add_argument("inputs", nargs="+", help="files or directories to describe")
    info.set_defaults(handler=command_info)
    return parser


def main(argv=None) -> int:
    parser = build_parser()
    options = parser.parse_args(argv)
    try:
        return options.handler(options)
    except SpriteError as error:
        print("sprites_opencode: %s" % error, file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
