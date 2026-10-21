"""Read the project's little-endian, 32-bit BRES/Collada 0,0,0,325 model data.

Layouts: Lib/Irrlicht/tools/{ResFile,ColladaConverter}/include;
component encodings: ColladaConverter/src/CColladaSource.h.
This module deliberately does not load native pointers or execute asset code.
"""
from pathlib import Path
import hashlib
import math
import struct


class BdaeError(ValueError):
    pass


class Bdae:
    COMPONENTS = {0: 'b', 1: 'B', 2: 'h', 3: 'H', 4: 'i', 5: 'I', 6: 'f'}

    def __init__(self, filename):
        self.path = Path(filename).resolve()
        self.data = self.path.read_bytes()
        if self.data[:8] != b'BRES\xfe\xff\0\0':
            raise BdaeError('Expected an unrelocated little-endian 32-bit BRES file')
        if self.u(8) != 32 or self.u(12) != len(self.data):
            raise BdaeError('Invalid BRES header/size')
        self.root = self.u(28)
        self.version = self.string(self.root)
        if self.version != '0,0,0,325':
            raise BdaeError(f'Unsupported Collada layout {self.version!r}; expected 0,0,0,325')
        self.up_axis = self.i(self.root + 4)
        self.fps = self.i(self.root + 12)
        if self.up_axis not in (1, 2) or not 1 <= self.fps <= 240:
            raise BdaeError('Unsupported coordinate system or sampling rate')
        self.sha256 = hashlib.sha256(self.data).hexdigest()
        self.clips = [dict(name=self.string(p), start_ms=self.i(p+4), end_ms=self.i(p+8))
                      for p in self.vector(self.root+28, 12)]
        self.tracks = [self.animation(p) for p in self.vector(self.root+16, 36)]
        self.nodes = []
        for scene in self.vector(self.root+108, 16):
            self.read_nodes(scene+8, None)
        if len({n['id'] for n in self.nodes}) != len(self.nodes):
            raise BdaeError('Duplicate scene node IDs')
        self.geometries = [self.geometry(p) for p in self.vector(self.root+76, 16)]
        self.skins = [self.skin(p) for p in self.vector(self.root+84, 12)]
        self.images = [dict(id=self.string(p), name=self.string(p+4), file=self.string(p+8))
                       for p in self.vector(self.root+52, 20)]
        self.events = []
        event_pointer = self.u(self.root+24)
        if event_pointer:
            times = self.source(event_pointer)['values']
            events = self.vector(event_pointer+12, 8)
            if len(times) != len(events):
                raise BdaeError('Event counts do not match')
            self.events = [dict(time_ms=t, names=[self.string(p) for p in self.vector(v, 4)])
                           for t, v in zip(times, events)]
        self.validate()

    def unpack(self, offset, fmt):
        length = struct.calcsize('<'+fmt)
        if offset < 0 or offset+length > len(self.data):
            raise BdaeError(f'Out-of-range field at {offset}')
        return struct.unpack_from('<'+fmt, self.data, offset)

    def u(self, offset):
        return self.unpack(offset, 'I')[0]

    def i(self, offset):
        return self.unpack(offset, 'i')[0]

    def string(self, field):
        pointer = self.u(field)
        if pointer == 0:
            return ''
        if pointer >= len(self.data):
            raise BdaeError('String pointer outside file')
        end = self.data.find(b'\0', pointer)
        if end < 0:
            raise BdaeError('Unterminated string')
        return self.data[pointer:end].decode('utf-8', errors='replace')

    def vector(self, field, stride):
        count, pointer = self.unpack(field, 'II')
        if count > 10_000_000 or (count and (not pointer or pointer+count*stride > len(self.data))):
            raise BdaeError(f'Invalid vector at {field}')
        return range(pointer, pointer+count*stride, stride)

    def values(self, field, component):
        fmt = self.COMPONENTS.get(component)
        if fmt is None:
            raise BdaeError(f'Unsupported scalar component {component}')
        locations = self.vector(field, struct.calcsize(fmt))
        return list(self.unpack(locations.start, fmt*len(locations)))

    def source(self, offset):
        component = self.i(offset)
        return dict(component=component, values=self.values(offset+4, component))

    def animation(self, offset):
        sources = list(self.vector(offset+4, 12))
        samplers = list(self.vector(offset+12, 20))
        channels = list(self.vector(offset+20, 16))
        if len(samplers) != 1 or len(channels) != 1:
            raise BdaeError('Only one sampler/channel per track is supported')
        interpolation, input_index, output_index, _, _ = self.unpack(samplers[0], '5i')
        if not (0 <= input_index < len(sources) and 0 <= output_index < len(sources)):
            raise BdaeError('Invalid animation source index')
        times = self.source(sources[input_index])['values']
        values = self.source(sources[output_index])['values']
        channel = channels[0]
        kind = self.i(channel+8)
        widths = {1: 3, 5: 4, 9: 1, 10: 3}
        if kind not in widths or interpolation not in (0, 1):
            raise BdaeError(f'Unsupported track type/interpolation: {kind}/{interpolation}')
        width = widths[kind]
        if not times or len(values) != len(times)*width or sorted(set(times)) != times:
            raise BdaeError('Invalid animation keys')
        return dict(name=self.string(offset), target=self.string(channel+4), kind=kind,
                    interpolation=interpolation, times=times,
                    values=[values[k:k+width] for k in range(0, len(values), width)])

    def read_nodes(self, field, parent, depth=0):
        if depth > 100:
            raise BdaeError('Scene hierarchy too deep or cyclic')
        for p in self.vector(field, 80):
            ident = self.string(p)
            values = self.unpack(p+12, '10f')
            self.nodes.append(dict(offset=p, id=ident, name=self.string(p+4), sid=self.string(p+8),
                                   parent=parent, position=values[:3], rotation=values[3:7],
                                   scale=values[7:], visible=bool(self.i(p+52)),
                                   instances=[self.unpack(q, 'II') for q in self.vector(p+64, 8)]))
            self.read_nodes(p+56, ident, depth+1)

    def geometry(self, offset):
        if self.i(offset+8) != 0:
            raise BdaeError('Only mesh geometries are supported')
        mesh = self.u(offset+12)
        if self.i(mesh) != 1:
            raise BdaeError('Only interleaved meshes are supported')
        count = self.i(mesh+4)
        interleaved = self.u(mesh+8)
        stride = self.i(interleaved)
        offsets = self.values(interleaved+4, 4)
        types = self.values(interleaved+12, 4)
        scales = self.values(interleaved+20, 5)
        data = self.u(interleaved+28)
        buffers = []
        for b in self.vector(mesh+12, 60):
            if self.i(b) != 0:
                raise BdaeError('Only triangle-list mesh buffers are supported')
            channels = self.unpack(b+12, '12b')
            indices = self.values(b+24, 3)
            if len(indices) != self.i(b+8)*3 or any(i >= count for i in indices):
                raise BdaeError('Invalid triangle indices')
            def attribute(source, width):
                if source < 0:
                    return None
                component = types[source] & 15
                if component not in self.COMPONENTS:
                    raise BdaeError('Unsupported interleaved component')
                fmt = self.COMPONENTS[component]*width
                if offsets[source]+struct.calcsize(fmt) > stride:
                    raise BdaeError('Vertex attribute exceeds stride')
                scale = self.unpack(scales[source], '6f') if scales[source] else None
                result = [self.unpack(data+i*stride+offsets[source], fmt) for i in range(count)]
                if scale:
                    result = [tuple(x*scale[j]+scale[j+3] for j, x in enumerate(v)) for v in result]
                return result
            buffers.append(dict(material=self.string(b+4), indices=indices,
                                positions=attribute(channels[0], 3),
                                uv=attribute(channels[4], 2), normals=attribute(channels[1], 3), colors=attribute(channels[10], 4)))
        return dict(id=self.string(offset), name=self.string(offset+4), vertices=count, buffers=buffers)

    def skin(self, offset):
        if self.i(offset) != 0:
            raise BdaeError('Only skeletal skin controllers are supported')
        p = self.u(offset+8)
        joints = [self.string(v) for v in self.vector(p+68, 4)]
        inverse_bind = self.values(p+76, 6)
        if len(inverse_bind) != 16*len(joints):
            raise BdaeError('Inverse bind matrix count does not match joints')
        weights = self.values(p+84, 6)
        counts = self.values(p+92, 1)
        pairs = self.values(p+100, 3)
        if len(pairs) != 2*sum(counts):
            raise BdaeError('Skin influence count mismatch')
        influences, cursor = [], 0
        for count in counts:
            row = []
            for _ in range(count):
                j, w = pairs[cursor:cursor+2]
                if j >= len(joints) or w >= len(weights):
                    raise BdaeError('Skin influence index out of range')
                row.append((j, weights[w]))
                cursor += 2
            if abs(sum(w for _, w in row)-1) > 1e-4:
                raise BdaeError('Unnormalized or unweighted vertex')
            influences.append(row)
        return dict(id=self.string(offset+4), geometry=self.string(p).lstrip('#'), joints=joints,
                    bind_shape=list(self.unpack(p+4, '16f')),
                    inverse_bind=[inverse_bind[i:i+16] for i in range(0, len(inverse_bind), 16)],
                    influences=influences)

    def validate(self):
        ids = {n['id'] for n in self.nodes}
        if any(t['target'] not in ids for t in self.tracks):
            raise BdaeError('Animation targets a missing node')
        geometry = {g['id']: g for g in self.geometries}
        sids = {n['sid'] for n in self.nodes}
        for skin in self.skins:
            if any(j not in sids for j in skin['joints']):
                raise BdaeError('Skin references a missing joint')
            if len(skin['influences']) != geometry[skin['geometry']]['vertices']:
                raise BdaeError('Skin/mesh vertex count differs')
        if any(c['end_ms'] <= c['start_ms'] for c in self.clips):
            raise BdaeError('Invalid clip duration')
        if any(not all(math.isfinite(v) for v in sample) for t in self.tracks for sample in t['values']):
            raise BdaeError('Non-finite animation values')

    def summary(self):
        return dict(source=str(self.path), sha256=self.sha256, layout=self.version, fps=self.fps,
                    nodes=len(self.nodes), animation_tracks=len(self.tracks), clips=self.clips,
                    events=self.events, meshes=[dict(name=g['name'], vertices=g['vertices'],
                    triangles=sum(len(b['indices'])//3 for b in g['buffers'])) for g in self.geometries],
                    skin_joints=[len(s['joints']) for s in self.skins])


if __name__ == '__main__':
    import argparse
    import json
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input')
    args = parser.parse_args()
    print(json.dumps(Bdae(args.input).summary(), indent=2))
