"""Whole-scene preservation checks, including morphs and secondary UVs."""
from pathlib import Path
import struct
import tempfile
import unittest
import xml.etree.ElementTree as ET

from bdae_scene import SceneData, write_meshes, verify_preserved
from bdae_reader import BdaeError

ROOT=Path(__file__).resolve().parents[2]
SOURCE=ROOT/'Data/Scenes/bin'


def identity_models(scene):
    result={}
    for g in scene.geometries:
        b=g['buffers'][0];skin=next((s for s in scene.skins if s['geometry']==g['id']),None)
        vertices=[]
        for i,position in enumerate(b['positions']):
            vertices.append(dict(position=list(position),normal=list(b['normals'][i]) if b['normals'] else [0,1,0],
                                 uv=list(b['uv'][i]) if b['uv'] else [0,0],uv1=list(b['uv1'][i]) if b['uv1'] else [0,0],
                                 color=list(b['colors'][i]) if b['colors'] else [255]*4,
                                 weights=skin['influences'][i] if skin else []))
        result[g['id']]=dict(vertices=vertices,buffers=[b['indices'] for b in g['buffers']],
                             joint_bounds=[[0.0]*6 for _ in skin['joints']] if skin else [])
    return result


class WholeSceneTests(unittest.TestCase):
    def test_diffuse_bindings_match_collada_effects_not_runtime_material_caches(self):
        ns={'c':'http://www.collada.org/2005/11/COLLADASchema'}
        for name in ['Dark_elf/elf','Castle/castle','Skeleton/skeleton']:
            source=SceneData(SOURCE/(name+'.bdae'))
            tree=ET.parse(ROOT/'Data/Scenes'/(name+'.dae')).getroot()
            effects={e.get('id'):e for e in tree.findall('c:library_effects/c:effect',ns)}
            for mat in tree.findall('c:library_materials/c:material',ns):
                if mat.get('id') not in source.materials:continue
                fx=effects[mat.find('c:instance_effect',ns).get('url').lstrip('#')]
                diffuse=fx.find('.//c:diffuse/c:texture',ns)
                if diffuse is None:continue
                params={p.get('sid'):p for p in fx.findall('.//c:newparam',ns)}
                surface=params[diffuse.get('texture')].find('c:sampler2D/c:source',ns).text
                image_id=params[surface].find('c:surface/c:init_from',ns).text
                image=next(i for i in tree.findall('c:library_images/c:image',ns) if i.get('id')==image_id)
                expected=Path(image.find('c:init_from',ns).text.replace('\\','/')).stem
                actual=source.images[source.materials[mat.get('id')]['image']]['file']
                self.assertEqual(Path(actual).stem,expected)

    def test_every_scene_is_readable_with_all_existing_features(self):
        scenes=[SceneData(p) for p in sorted(SOURCE.rglob('*.bdae'))]
        self.assertEqual(len(scenes),34)
        self.assertEqual(sum(bool(s.geometries) for s in scenes),33)
        self.assertEqual(sum(bool(s.morphs) for s in scenes),4)
        self.assertEqual(len(SceneData(SOURCE/'Skeleton/skeleton.bdae').skins),8)
        self.assertTrue(SceneData(SOURCE/'Hydra/hydra.bdae').splines)
        griffin=SceneData(SOURCE/'Griffin/griffin.bdae')
        self.assertTrue(any(c['start_ms']==c['end_ms'] for c in griffin.clips))

    def test_rewrite_preserves_all_original_nonmesh_bytes_and_channel_records(self):
        for name in ['Castle/castle','Gates/gate_02','Hydra/hydra','Skeleton/skeleton','Troll/troll']:
            with self.subTest(name=name),tempfile.TemporaryDirectory() as directory:
                source=SceneData(SOURCE/(name+'.bdae'));output=Path(directory)/'model.bdae'
                report=write_meshes(source,identity_models(source),output);actual=SceneData(output)
                self.assertEqual(actual.tracks,source.tracks)
                self.assertEqual(actual.clips,source.clips)
                self.assertEqual(actual.events,source.events)
                self.assertEqual(actual.materials,source.materials)
                self.assertEqual(report['original_animation_records_preserved'],len(source.animation_records))
                self.assertEqual([g['vertices'] for g in actual.geometries],[g['vertices'] for g in source.geometries])
                self.assertEqual([s['influences'] for s in actual.skins],[s['influences'] for s in source.skins])

    def test_secondary_uvs_and_vertex_alpha_survive(self):
        with tempfile.TemporaryDirectory() as directory:
            source=SceneData(SOURCE/'Gates/arch.bdae');path=Path(directory)/'arch.bdae'
            write_meshes(source,identity_models(source),path);actual=SceneData(path)
            for original,new in zip(source.geometries[0]['buffers'],actual.geometries[0]['buffers']):
                self.assertEqual(original['uv1'],new['uv1'])
                self.assertEqual(original['colors'],new['colors'])

    def test_preservation_check_detects_an_accidental_node_edit(self):
        with tempfile.TemporaryDirectory() as directory:
            source=SceneData(SOURCE/'Dark_elf/arrow.bdae');path=Path(directory)/'arrow.bdae'
            write_meshes(source,identity_models(source),path);data=bytearray(path.read_bytes())
            delta=(struct.unpack_from('<I',data,16)[0]-source.u(16))*4
            field=source.nodes[0]['offset']+delta+12
            struct.pack_into('<f',data,field,123.0);path.write_bytes(data)
            changed={g['offset']+12 for g in source.geometries}
            with self.assertRaisesRegex(BdaeError,'non-mesh scene payload changed'):
                verify_preserved(source,SceneData(path),changed)

    def test_morph_targets_must_have_corresponding_vertices(self):
        with tempfile.TemporaryDirectory() as directory:
            source=SceneData(SOURCE/'Gates/gate_01.bdae');models=identity_models(source)
            models[source.morphs[0]['targets'][0]]['vertices'].append(models[source.morphs[0]['targets'][0]]['vertices'][0])
            with self.assertRaisesRegex(BdaeError,'Morph topology correspondence changed'):
                write_meshes(source,models,Path(directory)/'gate.bdae')


if __name__=='__main__':unittest.main()
