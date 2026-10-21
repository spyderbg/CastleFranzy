"""Complete scene inventory plus mesh-only BRES replacement.

Original animation, morph, scene, material, camera and engine-extension records
are retained verbatim apart from relocated pointers. No animation is rebaked here.
"""
from pathlib import Path
import hashlib
import math
import struct

from bdae_reader import Bdae, BdaeError
from bdae_writer import Writer, bounds


class SceneData(Bdae):
    def __init__(self, filename):
        self.path = Path(filename).resolve()
        self.data = self.path.read_bytes()
        if self.data[:8] != b'BRES\xfe\xff\0\0' or self.u(8) != 32 or self.u(12) != len(self.data):
            raise BdaeError('Expected an unrelocated 32-bit little-endian BRES scene')
        self.root = self.u(28)
        self.version = self.string(self.root)
        if self.version != '0,0,0,325':
            raise BdaeError('Unsupported scene layout '+self.version)
        self.sha256 = hashlib.sha256(self.data).hexdigest()
        self.up_axis, self.fps = self.i(self.root+4), self.i(self.root+12)
        self.clips = [dict(name=self.string(p), start_ms=self.i(p+4), end_ms=self.i(p+8)) for p in self.vector(self.root+28,12)]
        self.animation_records = list(self.vector(self.root+16,36))
        self.tracks = []
        widths = {0:16, 1:3, 5:4, 10:3, 16:3}
        for p in self.animation_records:
            sources = list(self.vector(p+4,12)); samplers = list(self.vector(p+12,20)); channels = list(self.vector(p+20,16))
            if len(samplers) != len(channels):
                raise BdaeError('Mismatched animation sampler/channel count')
            for k,(sampler,channel) in enumerate(zip(samplers,channels)):
                interpolation, ti, vi, _, _ = self.unpack(sampler,'5i')
                times = self.source(sources[ti])['values']; values = self.source(sources[vi])['values']
                kind = self.i(channel+8); width = widths.get(kind,1)
                if not times or len(values) != len(times)*width or times != sorted(times):
                    raise BdaeError('Invalid scene animation source: '+self.string(p))
                self.tracks.append(dict(name=self.string(p), target=self.string(channel+4), kind=kind,
                                        interpolation=interpolation, times=times, reserved=self.u(channel+12),
                                        values=[values[j:j+width] for j in range(0,len(values),width)]))
        self.nodes=[]
        for p in self.vector(self.root+108,16): self.read_nodes(p+8,None)
        self.geometries=[]; self.splines=[]
        for p in self.vector(self.root+76,16):
            if self.i(p+8)==0:
                g=self.geometry(p);g['offset']=p
                m=self.u(p+12)
                for b,raw in zip(g['buffers'],self.vector(m+12,60)):
                    b['offset']=raw
                    # Secondary UVs are used by the arch and tornado.
                    channel=self.unpack(raw+12,'12b')[7]
                    b['uv1']=self.attribute(m,channel,2) if channel>=0 else None
                self.geometries.append(g)
            else:self.splines.append(dict(id=self.string(p), name=self.string(p+4),offset=p))
        self.skins=[];self.morphs=[]
        for p in self.vector(self.root+84,12):
            if self.i(p)==0:
                skin=self.skin(p);skin['offset']=p;self.skins.append(skin)
            elif self.i(p)==1:
                m=self.u(p+8)
                self.morphs.append(dict(id=self.string(p+4),source=self.string(m).lstrip('#'),
                                       targets=[self.string(t).lstrip('#') for t in self.vector(m+8,4)],
                                       weights=self.values(m+24,6),offset=p))
            else:raise BdaeError('Unsupported modular skin controller')
        self.images=[dict(id=self.string(p),name=self.string(p+4),file=self.string(p+8),type=self.i(p+12)) for p in self.vector(self.root+52,20)]
        self.materials={}
        effects=list(self.vector(self.root+60,92))
        for p in self.vector(self.root+68,64):
            effect=effects[self.i(p+24)]
            image=-1;color=(255,255,255,255)
            # Material diffuse caches are initialized at runtime. Resolve the
            # serialized effect -> texture -> sampler -> surface -> image chain.
            if self.i(effect+16)==1:
                textures=list(self.vector(self.u(effect+20),28))
                if textures:
                    sampler=list(self.vector(effect+76,4))[self.i(textures[0])]
                    surface=list(self.vector(effect+84,4))[self.i(sampler)]
                    image=self.i(surface)
            elif self.u(effect+20):color=self.unpack(self.u(effect+20),'4B')
            self.materials[self.string(p)]=dict(id=self.string(p),name=self.string(p+4),image=image,color=color,
                                               render=self.i(p+48),additive=self.i(p+28))
        self.events=[]
        p=self.u(self.root+24)
        if p:
            times=self.source(p)['values'];rows=self.vector(p+12,8)
            self.events=[dict(time_ms=t,names=[self.string(v) for v in self.vector(row,4)]) for t,row in zip(times,rows)]

    def attribute(self, mesh, channel, width):
        p=self.u(mesh+8); stride=self.i(p); offsets=self.values(p+4,4);types=self.values(p+12,4)
        scales=self.values(p+20,5);data=self.u(p+28)
        fmt=self.COMPONENTS[types[channel]&15]*width
        result=[self.unpack(data+i*stride+offsets[channel],fmt) for i in range(self.i(mesh+4))]
        if scales[channel]:
            scale=self.unpack(scales[channel],'6f');result=[tuple(v*scale[j]+scale[j+3] for j,v in enumerate(row)) for row in result]
        return result

    def bindings(self):
        """Geometry/controller URL -> instancing node and bound material IDs."""
        result={}
        for node in self.nodes:
            for kind,p in node['instances']:
                if kind not in (2,3):continue
                ident=self.string(p+4).lstrip('#')
                materials=[self.string(v+4).lstrip('#') for v in self.vector(p+12,20)]
                result.setdefault(ident,[]).append(dict(node=node['id'],materials=materials))
        return result


def verify_preserved(source, output, changed_fields):
    """Compare EVERY original non-mesh byte and original pointer destination."""
    count=source.u(16);old_end=32+count*4;new_end=32+output.u(16)*4;delta=new_end-old_end
    before=bytearray(source.data[old_end:]);after=bytearray(output.data[new_end:new_end+len(before)])
    for field in source.unpack(32,'I'*count):
        if field<old_end:continue
        if field not in changed_fields:
            target=source.u(field);expected=target+delta if target>=old_end else target
            if output.u(field+delta)!=expected:raise BdaeError(f'Preserved pointer changed: {field}')
        before[field-old_end:field-old_end+4]=b'\0'*4
        after[field-old_end:field-old_end+4]=b'\0'*4
    if before!=after:raise BdaeError('Original non-mesh scene payload changed')
    if source.clips!=output.clips or source.events!=output.events:raise BdaeError('Original clip/event metadata changed')
    return dict(original_payload_bytes_verified=len(before), original_relocations_verified=count,
                original_animation_records_preserved=len(source.animation_records),
                original_channels_preserved=len(source.tracks), clips_preserved=len(source.clips),events_preserved=len(source.events))


def write_meshes(source, models, output):
    w=Writer(source);changed=set()
    for geometry in source.geometries:
        model=models[geometry['id']];vertices=model['vertices'];buffers=model['buffers']
        if not vertices or len(vertices)>32768:raise BdaeError('Engine vertex limit exceeded: '+geometry['id'])
        has_uv1=any(b.get('uv1') is not None for b in geometry['buffers'])
        stride=44 if has_uv1 else 36
        mesh=w.alloc(44);w.put(mesh,'II',1,len(vertices));inter=w.alloc(32);w.ptr(mesh+8,inter);w.put(inter,'I',stride)
        w.array(inter+4,'i',[0,12,24,32]+([36] if has_uv1 else []))
        w.array(inter+12,'i',[6,6,6,1]+([6] if has_uv1 else []));w.array(inter+20,'I',[0]*(5 if has_uv1 else 4))
        data=w.alloc(stride*len(vertices));w.ptr(inter+28,data)
        for i,v in enumerate(vertices):
            floats=v['position']+v['normal']+v['uv']
            if not all(math.isfinite(x) for x in floats):raise BdaeError('Non-finite mesh attribute')
            w.put(data+i*stride,'8f4B',*floats,*v['color'])
            if has_uv1:w.put(data+i*stride+36,'2f',*v['uv1'])
        raw=w.alloc(60*len(buffers));w.vector(mesh+12,len(buffers),raw)
        all_points=[]
        for i,(original,indices) in enumerate(zip(geometry['buffers'],buffers)):
            b=raw+i*60
            w.data[b:b+60]=w.data[original['offset']:original['offset']+60]
            for field in list(w.relocations):
                if original['offset']<=field<original['offset']+60:w.relocations.add(b+field-original['offset'])
            w.put(b+8,'I',len(indices)//3)
            w.put(b+12,'12b',0,1,-1,-1,2,-1,-1,4 if has_uv1 else -1,-1,-1,3,-1)
            w.array(b+24,'H',indices);w.put(b+32,'hh',min(indices),max(indices))
            points=[vertices[j]['position'] for j in indices];all_points+=points
            w.put(b+36,'6f',*bounds(points))
        w.put(mesh+20,'6f',*bounds(all_points))
        w.ptr(geometry['offset']+12,mesh);changed.add(geometry['offset']+12)
        for skin in [s for s in source.skins if s['geometry']==geometry['id']]:
            original=source.u(skin['offset']+8);p=w.copy(original,116)
            w.ptr(skin['offset']+8,p);changed.add(skin['offset']+8)
            weights=[];pairs=[];counts=[];lookup={}
            for vertex in vertices:
                influence=vertex['weights'];counts.append(len(influence))
                if not influence or abs(sum(v for _,v in influence)-1)>1e-4:raise BdaeError('Invalid interpolated skin weights')
                for joint,value in influence:
                    encoded=struct.pack('<f',value)
                    if encoded not in lookup:lookup[encoded]=len(weights);weights.append(value)
                    if len(weights)>65536:raise BdaeError('Skin weight table exceeds engine limit')
                    pairs.extend([joint,lookup[encoded]])
            w.array(p+84,'f',weights);w.array(p+92,'B',counts);w.array(p+100,'H',pairs)
            boxes=w.alloc(24*len(skin['joints']))
            for i,box in enumerate(model['joint_bounds']):w.put(boxes+i*24,'6f',*box)
            w.vector(p+108,len(skin['joints']),boxes)
    Path(output).write_bytes(w.finish())
    reopened=SceneData(output)
    verification=verify_preserved(source,reopened,changed)
    # Morph targets must retain exactly corresponding vertices and triangle indices.
    by_id={g['id']:g for g in reopened.geometries}
    for morph in reopened.morphs:
        group=[by_id[morph['source']]]+[by_id[t] for t in morph['targets']]
        if len({g['vertices'] for g in group})!=1 or any(g['buffers'][0]['indices']!=group[0]['buffers'][0]['indices'] for g in group):
            raise BdaeError('Morph topology correspondence changed')
    return verification
