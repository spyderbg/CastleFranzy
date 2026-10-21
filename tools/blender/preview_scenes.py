"""Frame generated scene projects and render a gallery using Blender's CPU renderer."""
import argparse
import json
from pathlib import Path
import sys

import bpy
from mathutils import Vector

sys.path.insert(0,str(Path(__file__).resolve().parent))
from scene_tools import ROOT,frame_scene,material
from bdae_scene import SceneData


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--polish',action='store_true');parser.add_argument('--render',action='store_true')
    args=parser.parse_args(sys.argv[sys.argv.index('--')+1:]);config=json.loads((ROOT/'Data/Scenes/models.json').read_text())
    destination=ROOT/'build/verification/scenes/previews';destination.mkdir(parents=True,exist_ok=True)
    for entry in config['assets']:
        source=ROOT/'Data/Scenes'/entry['blend'];template=SceneData(ROOT/'Data/Scenes'/entry['template'])
        bpy.ops.wm.open_mainfile(filepath=str(source))
        if args.polish and bpy.context.scene.get('bdae_mesh_scene'):
            cache={}
            for previous in list(bpy.data.materials):
                ident=previous.get('bdae_material_id')
                if ident is not None:
                    updated=material(template,ident,cache);previous.user_remap(updated)
                    bpy.data.materials.remove(previous)
        # Preserve original node visibility and keep morph-target reference meshes hidden.
        bindings=template.bindings();nodes={n['id']:n for n in template.nodes}
        targets={t for m in template.morphs for t in m['targets']}
        for obj in bpy.context.scene.objects:
            ident=obj.get('bdae_geometry_id')
            if not ident:continue
            skin=next((s for s in template.skins if s['geometry']==ident),None);morph=next((m for m in template.morphs if m['source']==ident),None)
            link=bindings.get(skin['id'] if skin else morph['id'] if morph else ident,[None])[0]
            visible=ident not in targets and (nodes[link['node']]['visible'] if link else True)
            obj.hide_render=not visible;obj.hide_set(not visible)
        center,size=frame_scene()
        if args.polish and bpy.context.scene.get('bdae_mesh_scene'):
            bpy.context.preferences.filepaths.save_version=0
            bpy.ops.wm.save_as_mainfile(filepath=str(source),compress=True)
        if not args.render:continue
        for rig in [o for o in bpy.context.scene.objects if o.type=='ARMATURE' and o.animation_data]:
            preferred=next((c for word in ['walk','idle','loop','move'] for c in template.clips if word in c['name'].lower() and c['end_ms']>c['start_ms']),None)
            if preferred is None:preferred=next((c for c in template.clips if c['end_ms']>c['start_ms']),None)
            action=bpy.data.actions.get(preferred['name']) if preferred else rig.animation_data.action
            if action:
                rig.animation_data.action=action;rig.animation_data.action_slot=action.slots[0]
                bpy.context.scene.frame_set(round((action.frame_range[0]+action.frame_range[1])/2))
        center,size=frame_scene()
        scene=bpy.context.scene;scene.render.engine='CYCLES';scene.cycles.device='CPU';scene.cycles.samples=24;scene.cycles.use_denoising=True
        scene.render.threads_mode='FIXED';scene.render.threads=8
        scene.render.resolution_x=400;scene.render.resolution_y=400;scene.render.resolution_percentage=100
        scene.render.image_settings.file_format='PNG';scene.view_settings.view_transform='Standard'
        scene.world=scene.world or bpy.data.worlds.new('Preview world');scene.world.use_nodes=True
        scene.world.node_tree.nodes['Background'].inputs[0].default_value=(.06,.08,.11,1);scene.world.node_tree.nodes['Background'].inputs[1].default_value=.6
        camera=bpy.data.objects.new('Preview Camera',bpy.data.cameras.new('Preview Camera'));scene.collection.objects.link(camera);scene.camera=camera
        camera.location=center+Vector((3,-5,2.4)).normalized()*size*2
        camera.rotation_euler=(center-camera.location).to_track_quat('-Z','Y').to_euler();camera.data.type='ORTHO';camera.data.ortho_scale=size*1.06
        for name,relative,power in [('Key',(3,-4,5),400),('Fill',(-3,-1,3),180),('Rim',(0,4,4),300)]:
            light=bpy.data.objects.new(name,bpy.data.lights.new(name,'AREA'));scene.collection.objects.link(light)
            light.location=center+Vector(relative).normalized()*size*1.4;light.data.energy=power*(size/3)**2;light.data.shape='DISK';light.data.size=size
            light.rotation_euler=(center-light.location).to_track_quat('-Z','Y').to_euler()
        scene.render.filepath=str(destination/(entry['output'].replace('/','__').replace('.bdae','.png')))
        bpy.ops.render.render(write_still=True)
        print('SCENE_PREVIEW '+entry['blend'],flush=True)


if __name__=='__main__':main()
