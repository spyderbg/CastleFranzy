"""Write Blender-derived mesh/skin/animation data into a game BRES container.

Opaque engine extensions and materials are retained from the original asset.
All edited sections are newly serialized; the complete relocation table is rebuilt.
"""
import math
import struct

from bdae_reader import BdaeError


class Writer:
    def __init__(self, source):
        self.source = source
        self.data = bytearray(source.data)
        count, table = source.unpack(16, 'II')
        self.old_end = 32+count*4
        self.relocations = set(source.unpack(table, 'I'*count))
        if sorted(self.relocations)[:3] != [20, 24, 28]:
            raise BdaeError('Unexpected BRES header relocations')
        self.strings = {}

    def alloc(self, length):
        self.data.extend(b'\0'*(-len(self.data) % 4))
        offset = len(self.data)
        self.data.extend(b'\0'*length)
        return offset

    def put(self, offset, fmt, *values):
        struct.pack_into('<'+fmt, self.data, offset, *values)

    def ptr(self, field, target):
        self.put(field, 'I', target)
        if target:
            self.relocations.add(field)
        else:
            self.relocations.discard(field)

    def copy(self, offset, length):
        target = self.alloc(length)
        self.data[target:target+length] = self.data[offset:offset+length]
        for field in list(self.relocations):
            if offset <= field < offset+length:
                self.relocations.add(target+field-offset)
        return target

    def string(self, value):
        if value not in self.strings:
            raw = value.encode('utf-8')+b'\0'
            offset = self.alloc(4+len(raw))
            self.put(offset, 'I', len(raw))
            self.data[offset+4:offset+4+len(raw)] = raw
            self.strings[value] = offset+4
        return self.strings[value]

    def vector(self, field, count, target):
        self.put(field, 'I', count)
        self.ptr(field+4, target if count else 0)

    def array(self, field, fmt, values):
        values = list(values)
        offset = self.alloc(struct.calcsize('<'+fmt)*len(values)) if values else 0
        if values:
            self.put(offset, fmt*len(values), *values)
        self.vector(field, len(values), offset)
        return offset

    def finish(self):
        fields = sorted(self.relocations)
        new_end = 32+len(fields)*4
        delta = new_end-self.old_end
        def remap(offset):
            return offset+delta if offset >= self.old_end else offset
        result = bytearray(self.data[:32])+bytearray(len(fields)*4)+self.data[self.old_end:]
        struct.pack_into('<III', result, 12, len(result), len(fields), 32)
        for i, field in enumerate(fields):
            pointer = struct.unpack_from('<I', self.data, field)[0]
            if not pointer or pointer >= len(self.data):
                raise BdaeError(f'Invalid relocation: {field} -> {pointer}')
            struct.pack_into('<I', result, 32+i*4, remap(field))
            struct.pack_into('<I', result, remap(field), remap(pointer))
        return bytes(result)


def bounds(points):
    if not points:
        return [0.0]*6
    return [min(v[i] for v in points) for i in range(3)]+[max(v[i] for v in points) for i in range(3)]


def serialize(source, model):
    """Model is plain data produced by the Blender exporter, independent of bpy."""
    w = Writer(source)
    root = source.root
    vertices, indices = model['vertices'], model['indices']
    if not vertices or len(vertices) > 32768 or len(indices) % 3:
        raise BdaeError('The engine supports at most 32768 indexed vertices per model')
    if len(model['joints']) > 255:
        raise BdaeError('The game supports at most 255 skin joints')
    if any(i < 0 or i >= len(vertices) for i in indices):
        raise BdaeError('Triangle index out of range')
    geometry_offset = source.u(root+80)
    original_mesh = source.u(geometry_offset+12)
    original_buffer = source.u(original_mesh+16)
    mesh = w.alloc(44)
    w.put(mesh, 'II', 1, len(vertices))
    interleaved = w.alloc(32)
    w.ptr(mesh+8, interleaved)
    w.put(interleaved, 'I', 36)
    w.array(interleaved+4, 'i', [0, 12, 24, 32])
    w.array(interleaved+12, 'i', [6, 6, 6, 1])
    w.array(interleaved+20, 'I', [0, 0, 0, 0])
    data = w.alloc(36*len(vertices))
    w.ptr(interleaved+28, data)
    for i, vertex in enumerate(vertices):
        values = vertex['position']+vertex['normal']+vertex['uv']
        if not all(math.isfinite(v) for v in values):
            raise BdaeError('Non-finite vertex data')
        w.put(data+i*36, '8f4B', *values, *vertex.get('color', [255]*4))
    buffer = w.alloc(60)
    w.put(buffer, 'I', 0)
    w.ptr(buffer+4, source.u(original_buffer+4))
    w.put(buffer+8, 'I', len(indices)//3)
    w.put(buffer+12, '12b', 0, 1, -1, -1, 2, -1, -1, -1, -1, -1, 3, -1)
    w.array(buffer+24, 'H', indices)
    w.put(buffer+32, 'hh', min(indices), max(indices))
    box = bounds([v['position'] for v in vertices])
    w.put(buffer+36, '6f', *box)
    w.put(mesh+20, '6f', *box)
    w.vector(mesh+12, 1, buffer)
    w.ptr(geometry_offset+12, mesh)

    controller = source.u(root+88)
    skin = w.alloc(116)
    w.ptr(controller+8, skin)
    w.ptr(skin, w.string('#'+source.geometries[0]['id']))
    w.put(skin+4, '16f', *[1.0 if i % 5 == 0 else 0.0 for i in range(16)])
    joint_strings = w.alloc(4*len(model['joints']))
    for i, joint in enumerate(model['joints']):
        w.ptr(joint_strings+i*4, w.string(joint['sid']))
    w.vector(skin+68, len(model['joints']), joint_strings)
    w.array(skin+76, 'f', [v for j in model['joints'] for v in j['inverse_bind']])
    weights, pairs, counts = [], [], []
    weight_indices = {}
    for vertex in vertices:
        influences = vertex['weights']
        if not influences or len(influences) > 255 or abs(sum(v for _, v in influences)-1) > 1e-4:
            raise BdaeError('Unweighted vertex or invalid weight total')
        counts.append(len(influences))
        for joint, value in influences:
            if not 0 <= joint < len(model['joints']) or not math.isfinite(value) or value < 0:
                raise BdaeError('Invalid skin influence')
            encoded = struct.pack('<f', value)
            if encoded not in weight_indices:
                if len(weights) >= 65536:
                    raise BdaeError('The engine weight table exceeds 65535 entries')
                weight_indices[encoded] = len(weights)
                weights.append(value)
            pairs.extend([joint, weight_indices[encoded]])
    w.array(skin+84, 'f', weights)
    w.array(skin+92, 'B', counts)
    w.array(skin+100, 'H', pairs)
    # Conservative joint-local boxes cover every influenced bind vertex.
    boxes = w.alloc(24*len(model['joints']))
    for i, joint in enumerate(model['joints']):
        w.put(boxes+24*i, '6f', *joint['bounds'])
    w.vector(skin+108, len(model['joints']), boxes)

    node_records = {}
    for node in source.nodes:
        # Reader supplies original offsets so opaque attachments can be preserved.
        node_records[node['id']] = node['offset']
    nodes = {node['id']: node for node in model['nodes']}
    def write_nodes(parent, field):
        children = [n for n in model['nodes'] if n['parent'] == parent]
        records = w.alloc(80*len(children)) if children else 0
        w.vector(field, len(children), records)
        for i, node in enumerate(children):
            offset = records+i*80
            if node['id'] in node_records:
                original = node_records[node['id']]
                w.data[offset:offset+80] = w.data[original:original+80]
                for p in list(w.relocations):
                    if original <= p < original+80:
                        w.relocations.add(offset+p-original)
            for delta, key in [(0, 'id'), (4, 'name'), (8, 'sid')]:
                w.ptr(offset+delta, w.string(node[key]))
            w.put(offset+12, '10f', *node['position'], *node['rotation'], *node['scale'])
            w.put(offset+52, 'i', int(node.get('visible', True)))
            write_nodes(node['id'], offset+56)
    for node in model['nodes']:
        if node['parent'] and node['parent'] not in nodes:
            raise BdaeError('Missing node parent')
    # The original scene and instance URLs stay intact.
    visual = source.u(root+112)
    write_nodes(None, visual+8)

    animation_records = w.alloc(36*len(model['tracks']))
    w.vector(root+16, len(model['tracks']), animation_records)
    for i, track in enumerate(model['tracks']):
        p = animation_records+36*i
        w.ptr(p, w.string(track['name']))
        sources = w.alloc(24)
        w.vector(p+4, 2, sources)
        w.put(sources, 'I', 4)
        w.array(sources+4, 'i', track['times'])
        w.put(sources+12, 'I', 6)
        w.array(sources+16, 'f', [v for row in track['values'] for v in row])
        sampler = w.alloc(20)
        w.put(sampler, '5i', 1, 0, 1, -1, -1)
        w.vector(p+12, 1, sampler)
        channel = w.alloc(16)
        w.ptr(channel, w.string(''))
        w.ptr(channel+4, w.string(track['target']))
        w.put(channel+8, 'i', track['kind'])
        w.vector(p+20, 1, channel)
    clips = w.alloc(12*len(model['clips']))
    w.vector(root+28, len(model['clips']), clips)
    for i, clip in enumerate(model['clips']):
        w.ptr(clips+12*i, w.string(clip['name']))
        w.put(clips+12*i+4, 'ii', clip['start_ms'], clip['end_ms'])
    events = w.alloc(20)
    w.put(events, 'i', 4)
    w.array(events+4, 'i', [e['time_ms'] for e in model['events']])
    event_rows = w.alloc(8*len(model['events']))
    w.vector(events+12, len(model['events']), event_rows)
    for i, event in enumerate(model['events']):
        strings = w.alloc(4*len(event['names']))
        w.vector(event_rows+8*i, len(event['names']), strings)
        for j, name in enumerate(event['names']):
            w.ptr(strings+4*j, w.string(name))
    w.ptr(root+24, events)
    image = source.u(root+56)
    w.ptr(image+8, w.string(model['texture_file']))
    w.put(root+8, 'i', 0)
    w.put(root+12, 'i', model['fps'])
    return w.finish()
