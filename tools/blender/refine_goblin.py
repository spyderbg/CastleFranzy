"""Bake a denser editable Goblin surface while retaining UVs, weights and Actions.

blender -b Data/Scenes/Goblin/goblin_edit.blend --python-exit-code 1 \
  --python tools/blender/refine_goblin.py -- \
  --output Data/Scenes/Goblin/goblin_smooth.blend
"""
import argparse
import hashlib
import json
from pathlib import Path
import sys

import bmesh
import bpy


def mesh_counts(mesh):
    mesh.calc_loop_triangles()
    return dict(vertices=len(mesh.vertices), edges=len(mesh.edges),
                faces=len(mesh.polygons), triangles=len(mesh.loop_triangles))


def action_signature():
    """Hash clip curves and exact event metadata to catch accidental animation edits."""
    result = {}
    for action in bpy.data.actions:
        curves = []
        for layer in action.layers:
            for strip in layer.strips:
                for bag in strip.channelbags:
                    for curve in bag.fcurves:
                        curves.append([curve.data_path, curve.array_index,
                                       [(list(k.co), k.interpolation) for k in curve.keyframe_points]])
        result[action.name] = dict(curves=curves,
                                   markers=[(m.name, m.frame) for m in action.pose_markers],
                                   events=action.get('game_events_json', '[]'))
    return hashlib.sha256(json.dumps(result, sort_keys=True).encode()).hexdigest()


def refine(output, levels=2):
    source = Path(bpy.data.filepath).resolve()
    output = Path(output).resolve()
    if output.exists() or output.suffix.lower() != '.blend':
        raise ValueError('Choose an unused .blend output; existing artist projects are preserved')
    if not 2 <= levels <= 3:
        raise ValueError('Choose subdivision level 2 or 3')
    obj = bpy.data.objects['goblin']
    rig = bpy.data.objects['Goblin_Rig']
    before = mesh_counts(obj.data)
    animation_before = action_signature()
    bpy.ops.object.select_all(action='DESELECT')
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    if obj.mode != 'OBJECT':
        bpy.ops.object.mode_set(mode='OBJECT')
    armature = next(m for m in obj.modifiers if m.type == 'ARMATURE')
    subdiv = next((m for m in obj.modifiers if m.type == 'SUBSURF'), None)
    if subdiv is None:
        raise ValueError('Expected the first Goblin refinement as input')
    if list(obj.modifiers).index(subdiv) > list(obj.modifiers).index(armature):
        raise ValueError('Subdivision must precede the Armature modifier')
    # Keep rigid weapons crisp; relax body creases to remove the angular cage outline.
    bm = bmesh.new()
    bm.from_mesh(obj.data)
    crease = bm.edges.layers.float.get('crease_edge') or bm.edges.layers.float.new('crease_edge')
    deform = bm.verts.layers.deform.active
    names = {g.index: g.name for g in obj.vertex_groups}
    creased_weapons = relaxed_body = 0
    for edge in bm.edges:
        weapon = all(any('Prop' in names.get(group, '') and weight > 0.8
                         for group, weight in vertex[deform].items()) for vertex in edge.verts)
        if weapon:
            edge[crease] = 1.0
            creased_weapons += 1
        elif not edge.is_boundary:
            if edge[crease] > 0:
                relaxed_body += 1
            edge[crease] = min(edge[crease], 0.2)
    bm.to_mesh(obj.data)
    bm.free()
    subdiv.levels = subdiv.render_levels = levels
    subdiv.subdivision_type = 'CATMULL_CLARK'
    subdiv.show_viewport = subdiv.show_render = True
    # Applying only this modifier transfers interpolated deform weights and loop UVs.
    bpy.ops.object.modifier_apply(modifier=subdiv.name)
    mesh = obj.data
    mesh.name = 'Goblin_Smooth_Editable_Mesh'
    for polygon in mesh.polygons:
        polygon.use_smooth = True
    for vertex in mesh.vertices:
        total = sum(g.weight for g in vertex.groups if g.weight > 0)
        if total <= 0:
            raise ValueError(f'Unweighted vertex after refinement: {vertex.index}')
        for g in vertex.groups:
            obj.vertex_groups[g.group].add([vertex.index], g.weight/total, 'REPLACE')
    mesh.update()
    after = mesh_counts(mesh)
    assert after['vertices'] > 3000 and after['triangles'] > 6000
    assert mesh.uv_layers.active is not None and len(obj.vertex_groups) == 23
    assert len(rig.data.bones) == 25 and action_signature() == animation_before
    assert [m.type for m in obj.modifiers] == ['ARMATURE']
    report = dict(source=str(source), output=str(output), before=before, after=after,
                  subdivision_levels_applied=levels, relaxed_body_edges=relaxed_body,
                  protected_weapon_edges=creased_weapons,
                  actions_preserved=len(bpy.data.actions), animation_sha256=animation_before,
                  bones=25, weighted_joints=23)
    obj['refinement'] = 'Applied Catmull-Clark level %d; relaxed body creases; retained sharp weapons, UVs and skin weights.' % levels
    obj['refinement_report'] = json.dumps(report)
    obj['original_vertex_count'] = 257
    rig['bdae_template'] = 'Data/Scenes/bin/Goblin/goblin.bdae'
    rig['bdae_export_output'] = 'build/manual-models/Goblin/goblin.bdae'
    bpy.context.scene['bdae_authoring_source'] = 'Goblin/goblin_smooth.blend'
    notes = bpy.data.texts.get('README_Smooth_Goblin') or bpy.data.texts.new('README_Smooth_Goblin')
    notes.clear()
    notes.write('This is the denser, editable Goblin mesh. Subdivision is applied.\n'
                'Select goblin and press Tab to see the actual dense surface.\n'
                'Only the Armature modifier remains. All 12 named Actions are retained.\n'
                'Select Goblin_Rig and choose Actions in Dope Sheet > Action Editor.\n'
                'Save this file, close the game, and run ./rebuild_Linux.sh --resources-only.\n'
                'Build source registration: Data/Scenes/models.json.\n\n'+json.dumps(report, indent=2))
    # Make the saved project open on the actual new geometry in a useful solid view.
    for screen in bpy.data.screens:
        for area in screen.areas:
            if area.type == 'VIEW_3D':
                area.spaces.active.shading.type = 'SOLID'
                area.spaces.active.shading.light = 'STUDIO'
                area.spaces.active.shading.color_type = 'SINGLE'
                area.spaces.active.shading.single_color = (0.52, 0.60, 0.64)
                area.spaces.active.overlay.show_stats = True
    bpy.context.view_layer.update()
    output.parent.mkdir(parents=True, exist_ok=True)
    bpy.ops.wm.save_as_mainfile(filepath=str(output), compress=True)
    output.with_suffix('.mesh.json').write_text(json.dumps(report, indent=2)+'\n')
    print('GOBLIN_MESH_REFINED '+json.dumps(report), flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--levels', type=int, default=2)
    args = parser.parse_args(sys.argv[sys.argv.index('--')+1:])
    refine(args.output, args.levels)


if __name__ == '__main__':
    main()
