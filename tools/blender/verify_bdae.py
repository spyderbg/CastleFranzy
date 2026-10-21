"""Reopen a converted .blend and verify its rig/skin against the original BDAE.

blender --background Data/Scenes/Goblin/goblin.blend --python-exit-code 1 \
  --python tools/blender/verify_bdae.py -- --input Build/bin/Scenes/Goblin/goblin.bdae
Add --previews build/verification/goblin to render each named clip with Cycles CPU.
"""
import argparse
import json
from pathlib import Path
import sys

import bpy
from mathutils import Vector

sys.path.insert(0, str(Path(__file__).resolve().parent))
from import_bdae import Importer, Bdae, matrix


def loaded_importer(source):
    report = json.loads(bpy.context.scene['bdae_import_report'])
    if report['sha256'] != source.sha256:
        raise RuntimeError('The .blend was created from a different BDAE file')
    importer = Importer(source)
    importer.rig = bpy.data.objects['Goblin_Rig']
    importer.skin = source.skins[0]
    importer.names = {b['bdae_id']: b.name for b in importer.rig.data.bones}
    importer.bones = [n for n in source.nodes if n['id'] in importer.names]
    importer.rest = {ident: importer.rig.data.bones[name].matrix_local.copy()
                     for ident, name in importer.names.items()}
    sid = {n['sid']: n['id'] for n in source.nodes if n['sid']}
    importer.joint_ids = [sid[name] for name in importer.skin['joints']]
    importer.bind_world = {sid[s]: matrix(inv).inverted()
                           for s, inv in zip(importer.skin['joints'], importer.skin['inverse_bind'])}
    geometry = next(g for g in source.geometries if g['id'] == importer.skin['geometry'])
    importer.meshes = [(bpy.data.objects[geometry['name'] if index == 0 else f'{geometry["name"]}_{index}'], buffer)
                       for index, buffer in enumerate(geometry['buffers'])]
    importer.action_report = report['actions']
    importer.actions = {a['name']: bpy.data.actions[a['name']] for a in report['actions']}
    importer.slots = {name: action.slots[0] for name, action in importer.actions.items()}
    if len(importer.actions) != len(source.clips)+1:
        raise RuntimeError('Missing named clips or full-timeline Action')
    if not all(a.use_fake_user and a.asset_data is not None for a in importer.actions.values()):
        raise RuntimeError('Actions are not protected as persistent assets')
    if not all(o.data.uv_layers and len(o.vertex_groups) == len(importer.skin['joints']) for o, _ in importer.meshes):
        raise RuntimeError('UVs or vertex groups were lost')
    images = [n.image for m in bpy.data.materials if m.node_tree
              for n in m.node_tree.nodes if n.type == 'TEX_IMAGE' and n.image]
    if not images or not all(image.packed_file for image in images):
        raise RuntimeError('A diffuse texture is not packed in the .blend')
    return importer


def render_previews(importer, directory):
    directory.mkdir(parents=True, exist_ok=True)
    scene = bpy.context.scene
    scene.render.engine = 'CYCLES'
    scene.cycles.device = 'CPU'
    scene.cycles.samples = 24
    scene.cycles.use_denoising = False
    scene.render.resolution_x = 384
    scene.render.resolution_y = 384
    scene.render.resolution_percentage = 100
    scene.render.image_settings.file_format = 'PNG'
    scene.render.film_transparent = False
    scene.render.threads_mode = 'FIXED'
    scene.render.threads = 8
    scene.view_settings.view_transform = 'Standard'
    scene.world.use_nodes = True
    scene.world.node_tree.nodes['Background'].inputs[0].default_value = (0.12, 0.14, 0.18, 1)
    scene.world.node_tree.nodes['Background'].inputs[1].default_value = 0.5
    camera_data = bpy.data.cameras.new('Verification Camera')
    camera = bpy.data.objects.new('Verification Camera', camera_data)
    scene.collection.objects.link(camera)
    scene.camera = camera
    camera_data.type = 'ORTHO'
    for name, position, power, size in [('Key', (3, -4, 5), 450, 4),
                                       ('Fill', (-3, -1, 3), 220, 4),
                                       ('Rim', (0, 4, 4), 350, 3)]:
        light_data = bpy.data.lights.new(name, 'AREA')
        light_data.energy = power
        light_data.shape = 'DISK'
        light_data.size = size
        light = bpy.data.objects.new(name, light_data)
        scene.collection.objects.link(light)
        light.location = position
        light.rotation_euler = (Vector((0, 0, 0.8))-light.location).to_track_quat('-Z', 'Y').to_euler()
    for clip in importer.source.clips:
        importer.select_action(clip['name'])
        report = next(a for a in importer.action_report if a['name'] == clip['name'])
        frame = 1+(report['frame_end']-1)*0.5
        scene.frame_set(int(frame), subframe=frame-int(frame))
        bpy.context.view_layer.update()
        depsgraph = bpy.context.evaluated_depsgraph_get()
        points = []
        for obj, _ in importer.meshes:
            evaluated = obj.evaluated_get(depsgraph)
            mesh = evaluated.to_mesh()
            points.extend(evaluated.matrix_world @ v.co for v in mesh.vertices)
            evaluated.to_mesh_clear()
        low = Vector(tuple(min(p[i] for p in points) for i in range(3)))
        high = Vector(tuple(max(p[i] for p in points) for i in range(3)))
        center = (low+high)/2
        camera.location = center+Vector((3, -5, 2.8))
        camera.rotation_euler = (center-camera.location).to_track_quat('-Z', 'Y').to_euler()
        camera_data.ortho_scale = max((high-low).length*1.15, 1.8)
        scene.render.filepath = str(directory/(clip['name']+'.png'))
        bpy.ops.render.render(write_still=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input', type=Path, required=True)
    parser.add_argument('--previews', type=Path)
    args = parser.parse_args(sys.argv[sys.argv.index('--')+1:])
    importer = loaded_importer(Bdae(args.input))
    result = importer.verify()
    print('BDAE_REOPEN_VERIFIED '+json.dumps(result), flush=True)
    if args.previews:
        render_previews(importer, args.previews.resolve())


if __name__ == '__main__':
    main()
