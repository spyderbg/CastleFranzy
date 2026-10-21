"""Independent conformance checks using the original COLLADA XML as a reference."""
import math
from pathlib import Path
import struct
import tempfile
import unittest
import xml.etree.ElementTree as ET

from bdae_reader import Bdae, BdaeError

ROOT = Path(__file__).resolve().parents[2]


class GoblinReaderTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.file = ROOT/'Build/bin/Scenes/Goblin/goblin.bdae'
        cls.model = Bdae(cls.file)
        cls.xml = ET.parse(ROOT/'Data/Scenes/Goblin/goblin.dae').getroot()
        cls.ns = {'c': 'http://www.collada.org/2005/11/COLLADASchema'}

    def test_clip_names_and_timings_match_original_xml(self):
        clips = self.xml.findall('c:library_animation_clips/c:animation_clip', self.ns)
        self.assertEqual(len(clips), 12)
        self.assertEqual([c.get('id') for c in clips], [c['name'] for c in self.model.clips])
        for original, clip in zip(clips, self.model.clips):
            for field in ('start', 'end'):
                self.assertLessEqual(abs(float(original.get(field))*1000-clip[field+'_ms']), 1.01)

    def test_mesh_and_uv_values_match_original_xml(self):
        buffer = self.model.geometries[0]['buffers'][0]
        for name, field in [('goblin-mesh-positions', 'positions'),
                            ('goblin-mesh-map-channel1', 'uv')]:
            node = self.xml.find(f'.//c:source[@id="{name}"]/c:float_array', self.ns)
            expected = list(map(float, node.text.split()))
            actual = [v for row in buffer[field] for v in row]
            self.assertEqual(len(expected), len(actual))
            self.assertLess(max(abs(a-b) for a, b in zip(actual, expected)), 1e-7)
        self.assertEqual(len(buffer['indices'])//3, 282)
        self.assertEqual(self.model.geometries[0]['vertices'], 257)
        for values in buffer['colors']:
            self.assertEqual(values, (255, 255, 255, 255))

    def test_every_animation_key_matches_original_xml(self):
        animations = {a.get('id'): a for a in self.xml.findall('c:library_animations/c:animation', self.ns)}
        self.assertEqual(len(self.model.tracks), 35)
        for track in self.model.tracks:
            original = animations[track['name']]
            outputs = original.find('c:sampler/c:input[@semantic="OUTPUT"]', self.ns).get('source')[1:]
            node = original.find(f'c:source[@id="{outputs}"]/c:float_array', self.ns)
            values = list(map(float, node.text.split()))
            if track['kind'] == 5:
                self.assertEqual(len(values), len(track['values'])*4)
                for index, actual in enumerate(track['values']):
                    x, y, z, angle = values[index*4:index*4+4]
                    half = -angle*math.pi/360
                    # ColladaMax emits a zero axis for identity rotations.
                    norm = math.sqrt(x*x+y*y+z*z) or 1
                    expected = [x/norm*math.sin(half), y/norm*math.sin(half),
                                z/norm*math.sin(half), math.cos(half)]
                    self.assertAlmostEqual(abs(sum(a*b for a, b in zip(actual, expected))), 1, places=5)
            elif track['kind'] == 9:
                self.assertEqual(len(values), len(track['values']))
                for original, actual in zip(values, track['values']):
                    self.assertAlmostEqual(actual[0], -math.radians(original), places=5)
            elif track['kind'] == 1:
                actual = [v for row in track['values'] for v in row]
                self.assertEqual(len(values), len(actual))
                self.assertLess(max(abs(a-b) for a, b in zip(actual, values)), 1e-6)

    def test_skin_and_events_are_preserved(self):
        skin = self.model.skins[0]
        self.assertEqual(len(skin['joints']), 23)
        self.assertEqual(len(skin['influences']), 257)
        self.assertEqual(sum(len(row) for row in skin['influences']), 389)
        self.assertEqual(self.model.events, [dict(time_ms=666, names=['hit']),
                                            dict(time_ms=3000, names=['blood'])])
        self.assertEqual(len({n['id'] for n in self.model.nodes}), 27)

    def test_rejects_unknown_layout_and_bad_vector_bounds(self):
        original = self.file.read_bytes()
        with tempfile.TemporaryDirectory() as directory:
            target = Path(directory)/'bad.bdae'
            target.write_bytes(original.replace(b'0,0,0,325', b'0,0,0,326', 1))
            with self.assertRaisesRegex(BdaeError, 'Unsupported Collada layout'):
                Bdae(target)
            changed = bytearray(original)
            root = struct.unpack_from('<I', original, 28)[0]
            struct.pack_into('<I', changed, root+16, 100_000_000)
            target.write_bytes(changed)
            with self.assertRaisesRegex(BdaeError, 'Invalid vector'):
                Bdae(target)
            target.write_bytes(b'invalid')
            with self.assertRaisesRegex(BdaeError, 'BRES'):
                Bdae(target)


if __name__ == '__main__':
    unittest.main()
