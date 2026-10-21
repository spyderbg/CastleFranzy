"""Exercise binary relocation growth independently of Blender."""
from pathlib import Path
import struct
import tempfile
import unittest

from bdae_reader import Bdae, BdaeError
from bdae_writer import Writer

ROOT = Path(__file__).resolve().parents[2]


class WriterTests(unittest.TestCase):
    def setUp(self):
        self.source = Bdae(ROOT/'Data/Scenes/bin/Goblin/goblin.bdae')

    def read_back(self, data):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder)/'test.bdae'
            path.write_bytes(data)
            return Bdae(path)

    def test_unchanged_payload_keeps_mesh_skin_tracks_and_events(self):
        reopened = self.read_back(Writer(self.source).finish())
        for attribute in ['geometries', 'skins', 'tracks', 'clips', 'events']:
            self.assertEqual(getattr(reopened, attribute), getattr(self.source, attribute))

    def test_appended_strings_and_cloned_pointer_fields_survive_table_growth(self):
        writer = Writer(self.source)
        image = self.source.u(self.source.root+56)
        new_name = 'painted_texture_for_roundtrip.png'
        writer.ptr(image+8, writer.string(new_name))
        clone = writer.copy(image, 20)
        original_count = self.source.u(16)
        data = writer.finish()
        reopened = self.read_back(data)
        delta = (reopened.u(16)-original_count)*4
        self.assertGreater(delta, 0)
        self.assertEqual(reopened.string(clone+delta+8), new_name)
        self.assertEqual(reopened.images[0]['file'], new_name)
        self.assertEqual(reopened.tracks, self.source.tracks)
        fields = reopened.unpack(32, 'I'*reopened.u(16))
        self.assertEqual(list(fields), sorted(set(fields)))
        self.assertEqual(len(data), struct.unpack_from('<I', data, 12)[0])
        for field in fields:
            self.assertLess(field+3, len(data))
            self.assertTrue(0 < reopened.u(field) < len(data))

    def test_invalid_relocation_is_rejected(self):
        writer = Writer(self.source)
        writer.ptr(self.source.root+56, len(writer.data)+100)
        with self.assertRaisesRegex(BdaeError, 'Invalid relocation'):
            writer.finish()


if __name__ == '__main__':
    unittest.main()
