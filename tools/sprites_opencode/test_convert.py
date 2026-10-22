"""Run with: python3 tools/sprites_opencode/test_convert.py

The tests cover the legacy reader, the YAML reader and writer, the conversion
of every sprite of the project and the rules the game applies to YAML sprites.
When PyYAML is installed the generated files are also read with it and with a
port of the AuroraMX reader, which is how the editor sees them.
"""

from __future__ import annotations

import contextlib
import io
from pathlib import Path
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent))

import convert  # noqa: E402
import yaml_lite  # noqa: E402
from sprite_model import (  # noqa: E402
    MODULE_FIELDS,
    AnimationStep,
    Placement,
    SpriteAnimation,
    SpriteDocument,
    SpriteError,
    SpriteFrame,
    SpriteImage,
    SpriteModule,
    normalize_document,
    normalize_image_ids,
    validate_game_rules,
)
from sprite_text import decode_text, dump_sprite, parse_sprite  # noqa: E402
from sprite_yaml import dump_yaml, parse_yaml  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
SPRITES = ROOT / "Data" / "Sprites"

LEGACY = """// saved by AuroraGT v0.1.0.2 (SpriteEditor v0.1.0.5)
/*SPRITE*/ {
    VERSION 0001
    IMAGE 0x0000 ".\\a.tga" TRANSP 0x00FF00FF
    IMAGE 0x0001 "b.tga" ALPHA "b_mask.tga"
    MODULES
    {
        MD\t0x1000\tMD_IMAGE\t0\t3\t5\t7\t7\t"m"
        MD\t0x1001\tMD_FILL_RECT\t255\t4\t4
        MD\t0x1002\tMD_IMAGE\t1\t0\t0\t8\t8
    }
    FRAME "f"
    {
        0x2000
        RC 0 0 4 4
        FM 0x1000 -13 7 +FLIP_X
        FM 0x1001 1 2 FLAGS 0x20
    }
    FRAME ""
    {
        0x2001
        FM 0x2000 0 0 +HYPER_FM
    }
    ANIM "dizzy"
    {
        0x3000
        AF 0x2000 2 0 0 +ROT_90
    }
    SPRITE_END
}
"""


class LegacyReaderTests(unittest.TestCase):
    def setUp(self):
        self.document = parse_sprite(LEGACY)

    def test_reads_images_with_their_options(self):
        self.assertEqual(len(self.document.images), 2)
        self.assertEqual(self.document.images[0].file, ".\\a.tga")
        self.assertEqual(self.document.images[0].transparent, 0x00FF00FF)
        self.assertIsNone(self.document.images[0].alpha_file)
        self.assertEqual(self.document.images[1].alpha_file, "b_mask.tga")
        self.assertIsNone(self.document.images[1].transparent)

    def test_reads_modules_with_names(self):
        self.assertEqual(len(self.document.modules), 3)
        self.assertEqual(self.document.modules[0].type, "MD_IMAGE")
        self.assertEqual(self.document.modules[0].values, [0, 3, 5, 7, 7])
        self.assertEqual(self.document.modules[0].name, "m")
        self.assertEqual(self.document.modules[1].values, [255, 4, 4])

    def test_reads_frames_rects_and_flags(self):
        frame = self.document.frames[0]
        self.assertEqual(frame.name, "f")
        self.assertEqual(frame.rects, [(0, 0, 4, 4)])
        self.assertEqual(frame.parts[0].flags, 1)
        self.assertEqual(frame.parts[0].x, -13)
        self.assertEqual(frame.parts[1].flags, 0x20)

    def test_reads_hyperframes_and_animations(self):
        self.assertEqual(self.document.frames[1].parts[0].flags, 16)
        self.assertEqual(self.document.animations[0].frames[0].time, 2)
        self.assertEqual(self.document.animations[0].frames[0].flags, 4)

    def test_ignores_comments_and_sprite_end(self):
        self.assertEqual(self.document.extras, [])

    def test_rejects_broken_documents(self):
        with self.assertRaises(SpriteError):
            parse_sprite("/*SPRITE*/ { VERSION 0001 FRAME \"\" { 0x2000 FM 0x1000 } }")

    def test_rejects_text_in_no_known_encoding(self):
        with self.assertRaises(SpriteError):
            decode_text(b"IMAGE 0x0000 \"caf\x81.tga\"")

    def test_writes_a_document_the_reader_accepts(self):
        restored = parse_sprite(dump_sprite(self.document))
        self.assertEqual(restored, self.document)


class YamlReaderTests(unittest.TestCase):
    def test_reads_block_and_flow_collections(self):
        data = yaml_lite.parse(
            'a: 1\nb:\n  - { x: -2, y: 3 }\n  - name: "n"\n    flag: true\nc: "text"\n'
        )
        self.assertEqual(data["a"], 1)
        self.assertEqual(data["b"][0], {"x": -2, "y": 3})
        self.assertEqual(data["b"][1], {"name": "n", "flag": True})
        self.assertEqual(data["c"], "text")

    def test_reads_comments_and_block_scalars(self):
        data = yaml_lite.parse('# comment\na: "x # not a comment"  # real comment\nb: |\n  one\n  two\n')
        self.assertEqual(data["a"], "x # not a comment")
        self.assertEqual(data["b"], "one\ntwo\n")

    def test_reads_sequences_at_the_level_of_their_key(self):
        data = yaml_lite.parse("a:\n- 1\n- 2\n")
        self.assertEqual(data["a"], [1, 2])

    def test_rejects_duplicate_keys(self):
        with self.assertRaises(yaml_lite.YamlError):
            yaml_lite.parse("a: 1\na: 2\n")


class YamlWriterTests(unittest.TestCase):
    def test_writes_ids_as_quoted_hexadecimal_strings(self):
        document = parse_sprite(LEGACY)
        text = dump_yaml(document)
        self.assertIn('id: "0x1000"', text)
        self.assertIn('image: "0x0000"', text)
        self.assertIn('transparent: "0x00FF00FF"', text)
        self.assertIn('version: "0001"', text)

    def test_writes_placements_as_flow_mappings(self):
        document = parse_sprite(LEGACY)
        text = dump_yaml(document)
        self.assertIn('{ module: "0x1000", x: -13, y: 7, flags: [FLIP_X] }', text)
        self.assertIn('{ module: "0x1001", x: 1, y: 2, extraFlags: 32 }', text)
        self.assertIn('{ module: "0x2000", x: 0, y: 0, flags: [HYPER_FM] }', text)
        self.assertIn('{ frame: "0x2000", time: 2, x: 0, y: 0, flags: [ROT_90] }', text)

    def test_round_trips_the_document(self):
        document = parse_sprite(LEGACY)
        self.assertEqual(parse_yaml(dump_yaml(document)), document)

    def test_keeps_the_legacy_image_ids_when_asked(self):
        document = parse_sprite(LEGACY)
        text = dump_yaml(document)
        self.assertIn('id: "0x0001"', text)
        self.assertIn('image: "0x0001"', text)

    def test_writes_every_section_the_game_needs(self):
        document = parse_sprite(LEGACY)
        text = dump_yaml(document)
        for section in ("images:", "modules:", "frames:", "animations:"):
            self.assertIn(section, text)

    def test_writes_empty_sections_as_empty_lists(self):
        document = parse_sprite(LEGACY)
        document.animations = []
        text = dump_yaml(document)
        self.assertIn("animations: []", text)
        self.assertEqual(parse_yaml(text), document)


SIMPLE = """// a sprite the game can load
/*SPRITE*/ {
    VERSION 0001
    IMAGE 0x0000 "a.tga" TRANSP 0x00FF00FF
    MODULES
    {
        MD\t0x1000\tMD_IMAGE\t0\t0\t0\t8\t8
    }
    FRAME ""
    {
        0x2000
        FM 0x1000 0 0
    }
}
"""


class GameRuleTests(unittest.TestCase):
    """The rules the game applies when it loads a YAML sprite."""

    def test_accepts_a_converted_document(self):
        document = parse_sprite(SIMPLE)
        normalize_document(document)
        self.assertEqual(validate_game_rules(document), [])

    def test_reports_a_second_atlas(self):
        document = parse_sprite(LEGACY)
        problems = validate_game_rules(document)
        self.assertTrue(any("one image atlas" in problem for problem in problems))

    def test_reports_a_module_without_an_image(self):
        document = parse_sprite(LEGACY)
        document.modules[0].values[0] = -1
        problems = validate_game_rules(document)
        self.assertTrue(any("missing image" in problem for problem in problems))

    def test_reports_a_module_that_is_not_an_image(self):
        document = parse_sprite(LEGACY)
        document.modules[0].type = "MD_FILL_RECT"
        problems = validate_game_rules(document)
        self.assertTrue(any("MD_IMAGE" in problem for problem in problems))

    def test_reports_the_hyper_flag_in_an_animation(self):
        document = parse_sprite(LEGACY)
        document.animations[0].frames[0].flags |= 16
        problems = validate_game_rules(document)
        self.assertTrue(any("HYPER_FM" in problem for problem in problems))

    def test_reports_a_module_outside_the_runtime_limits(self):
        document = parse_sprite(LEGACY)
        document.modules[0].values[3] = 40000
        problems = validate_game_rules(document)
        self.assertTrue(any("32767" in problem for problem in problems))

    def test_reports_a_cyclic_hyperframe(self):
        document = parse_sprite(LEGACY)
        document.frames[0].parts.append(Placement(target=document.frames[0].id, flags=16))
        problems = validate_game_rules(document)
        self.assertTrue(any("cyclic" in problem for problem in problems))

    def test_reports_flag_bits_the_game_ignores(self):
        document = parse_sprite(LEGACY)
        problems = validate_game_rules(document)
        self.assertTrue(any("0x20" in problem and "ignores" in problem for problem in problems))

    def test_warns_about_a_module_without_an_image(self):
        document = parse_sprite(LEGACY)
        document.modules[0].values[0] = -1
        notes = normalize_document(document)
        self.assertTrue(any(note.startswith("warning: module") for note in notes))
        self.assertEqual(document.modules[0].values[0], 0)


class ConversionTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.directory = Path(self.temporary.name)
        self.source = self.directory / "sample.sprite"
        self.source.write_text(LEGACY, encoding="utf-8")

    def tearDown(self):
        self.temporary.cleanup()

    def options(self, **overrides):
        options = type("Options", (), {})()
        options.verify = True
        options.overwrite = False
        options.auroramx = False
        options.quiet = True
        for name, value in overrides.items():
            setattr(options, name, value)
        return options

    def test_writes_a_yaml_file_next_to_the_source(self):
        convert.convert_document(self.source, self.directory / "sample.yaml", self.options())
        text = (self.directory / "sample.yaml").read_text(encoding="utf-8")
        self.assertTrue(text.startswith("# Converted from sample.sprite"))
        self.assertIn("sprite:", text)

    def test_refuses_to_replace_a_file(self):
        target = self.directory / "sample.yaml"
        target.write_text("old", encoding="utf-8")
        with self.assertRaises(SpriteError):
            convert.convert_document(self.source, target, self.options())

    def test_replaces_a_file_when_asked(self):
        target = self.directory / "sample.yaml"
        target.write_text("old", encoding="utf-8")
        convert.convert_document(self.source, target, self.options(overwrite=True))
        self.assertIn("sprite:", target.read_text(encoding="utf-8"))

    def test_normalises_the_image_ids(self):
        document = parse_sprite(LEGACY)
        document.images[1].id = 0x0007
        notes = normalize_image_ids(document)
        self.assertEqual(len(notes), 1)
        self.assertIn("0x0007", notes[0])
        self.assertEqual([image.id for image in document.images], [0, 1])
        self.assertEqual(document.modules[2].values[0], 1)


class CommandLineTests(unittest.TestCase):
    """The commands of the tool itself."""

    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.directory = Path(self.temporary.name)
        self.source = self.directory / "sample.sprite"
        self.source.write_text(LEGACY, encoding="utf-8")

    def tearDown(self):
        self.temporary.cleanup()

    def run_tool(self, *arguments):
        """Run the command line tool and capture what it prints."""
        output = io.StringIO()
        with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            code = convert.main(list(arguments))
        return code, output.getvalue()

    def test_converts_a_file_next_to_the_source(self):
        code, _ = self.run_tool("convert", str(self.source), "-q")
        self.assertEqual(code, 0)
        self.assertTrue((self.directory / "sample.yaml").is_file())

    def test_refuses_to_replace_a_file(self):
        self.run_tool("convert", str(self.source), "-q")
        code, output = self.run_tool("convert", str(self.source), "-q")
        self.assertEqual(code, 1)
        self.assertIn("exists", output)

    def test_replaces_a_file_when_asked(self):
        self.run_tool("convert", str(self.source), "-q")
        code, _ = self.run_tool("convert", str(self.source), "-q", "--overwrite")
        self.assertEqual(code, 0)

    def test_writes_into_another_directory(self):
        target = self.directory / "out"
        code, _ = self.run_tool("convert", str(self.source), "-o", str(target), "-q")
        self.assertEqual(code, 0)
        self.assertTrue((target / "sample.yaml").is_file())

    def test_reverses_a_yaml_file(self):
        self.run_tool("convert", str(self.source), "-q")
        target = self.directory / "back"
        code, _ = self.run_tool(
            "reverse", str(self.directory / "sample.yaml"), "-o", str(target), "-q"
        )
        self.assertEqual(code, 0)
        self.assertTrue((target / "sample.sprite").is_file())

    def test_verify_reports_a_document_the_game_cannot_load(self):
        code, output = self.run_tool("verify", str(self.source))
        self.assertEqual(code, 1)
        self.assertIn("atlas", output)

    def test_verify_reports_the_two_atlases_of_the_sample(self):
        self.run_tool("convert", str(self.source), "-q")
        code, output = self.run_tool("verify", str(self.directory / "sample.yaml"))
        self.assertEqual(code, 1)
        self.assertIn("atlas", output)

    def test_info_describes_a_document(self):
        code, output = self.run_tool("info", str(self.source))
        self.assertEqual(code, 0)
        self.assertIn("2 images", output)

    def test_reverses_a_yml_file(self):
        self.run_tool("convert", str(self.source), "-q")
        (self.directory / "sample.yaml").rename(self.directory / "sample.yml")
        target = self.directory / "back"
        code, _ = self.run_tool("reverse", str(self.directory / "sample.yml"), "-o", str(target), "-q")
        self.assertEqual(code, 0)
        self.assertTrue((target / "sample.sprite").is_file())

    def test_info_reports_a_broken_file_and_continues(self):
        (self.directory / "broken.sprite").write_text("not a sprite", encoding="utf-8")
        code, output = self.run_tool("info", str(self.directory))
        self.assertEqual(code, 1)
        self.assertIn("broken.sprite", output)
        self.assertIn("sample.sprite", output)

    def test_verify_reports_flag_bits_the_game_ignores(self):
        code, output = self.run_tool("verify", str(self.source))
        self.assertEqual(code, 1)
        self.assertIn("extraFlags", output)


class ProjectSpriteTests(unittest.TestCase):
    """Every sprite of the project has to survive the conversion."""

    def sprites(self):
        return sorted(path for path in SPRITES.rglob("*.sprite") if path.is_file())

    def test_the_project_has_sprites(self):
        self.assertGreaterEqual(len(self.sprites()), 29)

    def test_every_sprite_converts_and_round_trips(self):
        for path in self.sprites():
            with self.subTest(sprite=path.name):
                document = parse_sprite(decode_text(path.read_bytes()))
                normalize_document(document)
                text = dump_yaml(document)
                self.assertEqual(parse_yaml(text), document)
                self.assertEqual(validate_game_rules(document), [])

    def test_every_sprite_survives_the_legacy_round_trip(self):
        for path in self.sprites():
            with self.subTest(sprite=path.name):
                document = parse_sprite(decode_text(path.read_bytes()))
                self.assertEqual(parse_sprite(dump_sprite(document)), document)

    def test_every_sprite_is_valid_yaml(self):
        try:
            import yaml
        except ImportError:
            self.skipTest("PyYAML is not installed")
        for path in self.sprites():
            with self.subTest(sprite=path.name):
                document = parse_sprite(decode_text(path.read_bytes()))
                normalize_document(document)
                data = yaml.safe_load(dump_yaml(document))
                self.assertIn("sprite", data)

    def test_the_auroramx_cli_accepts_every_project_sprite(self):
        """Load every converted sprite with the real AuroraMX command line tool."""
        if not convert.AURORAMX_CLI.is_file():
            self.skipTest("the AuroraMX command line tool is not built")
        with tempfile.TemporaryDirectory() as temporary:
            for path in self.sprites():
                with self.subTest(sprite=path.name):
                    document = parse_sprite(decode_text(path.read_bytes()))
                    normalize_document(document)
                    target = Path(temporary) / (path.stem + ".yaml")
                    target.write_text(dump_yaml(document), encoding="utf-8")
                    self.assertIsNone(convert.check_with_auroramx(target))

    def test_every_sprite_is_read_the_way_auroramx_reads_it(self):
        """Read the result with PyYAML and with the rules of the editor."""
        try:
            import yaml
        except ImportError:
            self.skipTest("PyYAML is not installed")
        for path in self.sprites():
            with self.subTest(sprite=path.name):
                document = parse_sprite(decode_text(path.read_bytes()))
                normalize_document(document)
                data = yaml.safe_load(dump_yaml(document))
                self.assertEqual(auroramx_view(data["sprite"]), document)


def auroramx_number(value):
    """Read a number the way the editor does, with the semantics of Number()."""
    if isinstance(value, bool):
        raise SpriteError("Expected an integer, found %r" % (value,))
    if isinstance(value, int):
        return value
    if isinstance(value, float):
        if not value.is_integer():
            raise SpriteError("Expected an integer, found %r" % (value,))
        return int(value)
    if isinstance(value, str):
        text = value.strip()
        try:
            if text[:2].lower() == "0x":
                return int(text[2:], 16)
            return int(text, 10)
        except ValueError:
            raise SpriteError("Expected an integer, found %r" % (value,)) from None
    raise SpriteError("Expected an integer, found %r" % (value,))


def auroramx_view(sprite) -> SpriteDocument:
    """Apply the rules of the AuroraMX reader to a parsed YAML mapping.

    This mirrors ``parseSpriteYaml`` of ``Tools/AuroraMX/backend/format.ts``:
    numbers are read with the semantics of ``Number()``, a module image is
    used as an *index* and a part that carries a ``frame`` key is a
    hyperframe.
    """
    document = SpriteDocument()
    document.version = str(sprite.get("version", "0001"))
    for entry in sprite.get("images") or []:
        image = SpriteImage(
            id=auroramx_number(entry["id"]),
            file=str(entry["file"]),
        )
        if entry.get("alphaFile") is not None:
            image.alpha_file = str(entry["alphaFile"])
        if entry.get("transparent") is not None:
            image.transparent = auroramx_number(entry["transparent"])
        document.images.append(image)
    for entry in sprite.get("modules") or []:
        fields = MODULE_FIELDS[str(entry["type"])]
        values = [auroramx_number(entry[name]) for name in fields]
        document.modules.append(
            SpriteModule(
                id=auroramx_number(entry["id"]),
                type=str(entry["type"]),
                values=values,
                name="" if entry.get("name") is None else str(entry["name"]),
            )
        )
    for entry in sprite.get("frames") or []:
        parts = []
        for part in entry.get("parts") or []:
            hyper = "frame" in part
            target = part["frame"] if hyper else part["module"]
            flags = 16 if hyper else 0
            for name in part.get("flags") or []:
                flags |= {"FLIP_X": 1, "FLIP_Y": 2, "ROT_90": 4, "HYPER_FM": 16}[name]
            if part.get("extraFlags") is not None:
                flags |= auroramx_number(part["extraFlags"])
            parts.append(
                Placement(
                    target=auroramx_number(target),
                    x=auroramx_number(part.get("x", 0)),
                    y=auroramx_number(part.get("y", 0)),
                    flags=flags,
                )
            )
        document.frames.append(
            SpriteFrame(
                id=auroramx_number(entry["id"]),
                name="" if entry.get("name") is None else str(entry["name"]),
                rects=[
                    (
                        auroramx_number(rect["x1"]),
                        auroramx_number(rect["y1"]),
                        auroramx_number(rect["x2"]),
                        auroramx_number(rect["y2"]),
                    )
                    for rect in entry.get("rects") or []
                ],
                parts=parts,
            )
        )
    for entry in sprite.get("animations") or []:
        steps = []
        for step in entry.get("frames") or []:
            flags = 0
            for name in step.get("flags") or []:
                flags |= {"FLIP_X": 1, "FLIP_Y": 2, "ROT_90": 4, "HYPER_FM": 16}[name]
            if step.get("extraFlags") is not None:
                flags |= auroramx_number(step["extraFlags"])
            steps.append(
                AnimationStep(
                    target=auroramx_number(step["frame"]),
                    time=auroramx_number(step.get("time", 0)),
                    x=auroramx_number(step.get("x", 0)),
                    y=auroramx_number(step.get("y", 0)),
                    flags=flags,
                )
            )
        document.animations.append(
            SpriteAnimation(
                id=auroramx_number(entry["id"]),
                name="" if entry.get("name") is None else str(entry["name"]),
                frames=steps,
            )
        )
    return document


if __name__ == "__main__":
    unittest.main()
