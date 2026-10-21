"""Blender integration test: topology, reweighting, new bone/clip, edited pose and painted texture.

blender -b Data/Scenes/Goblin/goblin_smooth.blend --python-exit-code 1 \
  --python tools/blender/test_export_roundtrip.py
Creates test assets only below build/verification/blender-export/edited-test/.
"""
from pathlib import Path
import sys
import math
import json
import bpy
from mathutils import Quaternion

sys.path.insert(0, str(Path(__file__).resolve().parent))
from export_bdae import export_model
from bdae_reader import Bdae

root=Path(__file__).resolve().parents[2]
rig=bpy.data.objects['Goblin_Rig']; obj=bpy.data.objects['goblin']
bpy.context.view_layer.objects.active=rig
bpy.ops.object.mode_set(mode='EDIT')
head=rig.data.edit_bones['Bip01_Head']
bone=rig.data.edit_bones.new('BDAE_Extra')
bone.length=0.1;bone.matrix=head.matrix.copy();bone.parent=head;bone.use_deform=True
bpy.ops.object.mode_set(mode='OBJECT')
new_group=obj.vertex_groups.new(name='BDAE_Extra'); head_group=obj.vertex_groups['Bip01_Head']
changed=0
for vertex in obj.data.vertices:
    weight=next((g.weight for g in vertex.groups if g.group==head_group.index),0)
    if weight>0.5:
        head_group.add([vertex.index],weight*0.75,'REPLACE')
        new_group.add([vertex.index],weight*0.25,'REPLACE')
        changed+=1
assert changed>0
original=bpy.data.actions['gob_vihren_walk_03']
action=original.copy();action.name='gob_editor_test';action['bdae_export']=True
action.use_frame_range=True;action.frame_start=1;action.frame_end=46
rig.animation_data.action=action;rig.animation_data.action_slot=action.slots[0]
bpy.context.scene.frame_set(16);bpy.context.view_layer.update()
head=rig.pose.bones['Bip01_Head']
head.rotation_quaternion=head.rotation_quaternion @ Quaternion((0,0,1),math.radians(20))
head.keyframe_insert('rotation_quaternion',frame=16,group=head.name)
texture=next(n for n in obj.data.materials[0].node_tree.nodes if n.type=='TEX_IMAGE')
texture.image=texture.image.copy()
texture.image.pixels[0:4]=(0.1,0.8,0.4,1)
texture.image.update()
image_state=(texture.image.filepath_raw,texture.image.file_format,texture.image.is_dirty)
output=root/'build/verification/blender-export/edited-test/goblin.bdae'
report=export_model(root/'Data/Scenes/bin/Goblin/goblin.bdae',output,rig.name)
assert image_state==(texture.image.filepath_raw,texture.image.file_format,texture.image.is_dirty)
model=Bdae(output)
assert report['bones']==26 and report['joints']==24
assert len(model.clips)==13 and model.clips[-1]['name']=='gob_editor_test'
assert model.clips[-1]['end_ms']-model.clips[-1]['start_ms']==1500
assert report['triangles']==6720
assert len(obj.data.vertices)==3432
assert 'Blender_BDAE_Extra' in model.skins[0]['joints']
assert model.events==[dict(time_ms=666,names=['hit']),dict(time_ms=3000,names=['blood'])]
image=bpy.data.images.load(str(output.parent/report['texture']),check_existing=False)
pixel=image.pixels[:4]
assert abs(pixel[0]-0.1)<0.01 and abs(pixel[1]-0.8)<0.01 and abs(pixel[2]-0.4)<0.01,pixel
print('EDITED_ROUNDTRIP_OK '+json.dumps(dict(reweighted_vertices=changed,bones=26,joints=24,clips=13,triangles=report['triangles'],painted_pixel=list(pixel),verification=report['verification'])),flush=True)
