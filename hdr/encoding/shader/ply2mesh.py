#!/usr/bin/env python
#
# Converter from Stanford's PLY format to a binary '.mesh'
# format that is easy to load in OpenGL even when feeling
# particularly lazy. The format is binary, with floating
# point numbers stored in IEEE single-precision and integers
# in big-endian.
#
# The header consists simply of two 32-bit integers giving
# first the number of vertices then the number of triangles.
# These are followed by all vertices stored as groups of
# three floats, then all triangles as groups of three 32-bit
# indices.
#
# This only supports a subset of the PLY format. In particular,
# only ASCII files are supported and vertex_indices must each
# be lists of three, signifying triangles.
#
# usage: ply2mesh.py input.ply output.mesh
#
# Micah Dowty <micah@navi.cx>
#

import sys, struct

class Element:
    def __init__(self, name, count):
        self.name = name
        self.count = count
        self.properties = []

    def parse_property(self, tokens):
        # Store a (name, type, args) tuple for each property
        name = tokens[-1]
        type = tokens[1]
        args = tokens[2:-1]
        self.properties.append((name, type, args))

    def read(self, stream):
        elements = []
        for i in xrange(self.count):
            tokens = stream.readline().strip().split()
            # For each property, find a handler to extract items from this line's tokens
            properties = {}
            for name, type, args in self.properties:
                properties[name] = self.extract(type, tokens, args)
            elements.append(properties)
        return elements

    def extract(self, type, tokens, args=None):
        return getattr(self, "extract_%s" % type)(tokens, args)

    def extract_float(self, tokens, args=None):
        f = float(tokens[0])
        del tokens[0]
        return f

    def extract_uchar(self, tokens, args=None):
        i = int(tokens[0])
        del tokens[0]
        return i

    def extract_int(self, tokens, args=None):
        i = int(tokens[0])
        del tokens[0]
        return i

    def extract_list(self, tokens, args):
        sizeType, itemType = args
        length = self.extract(sizeType, tokens)
        items = []
        for i in xrange(length):
            items.append(self.extract(itemType, tokens))
        return items

def read_ply(stream):
    """Given a file-like object, this reads PLY data and returns
       a Python representation of it. At the top level is a dictionary
       mapping element types to a list of all such elements. Each element
       is a dictionary mapping property names to property values.
       """

    # Parse the header, extracting Element instances
    elements = []
    while 1:
        tokens = stream.readline().strip().split()
        if tokens:
            if tokens[0] == 'element':
                elements.append(Element(tokens[1], int(tokens[2])))
            elif tokens[0] == 'property':
                if elements:
                    elements[-1].parse_property(tokens)
            elif tokens[0] == 'end_header':
                break

    # Read each element, adding up the parsed results.
    results = {}
    for element in elements:
        results[element.name] = element.read(stream)
    return results

def write_mesh(stream, model):
    """Write a pythonized PLY-style mesh out to our simple binary '.mesh' file"""
    vertices = model['vertex']
    triangles = model['face']
    stream.write(struct.pack(">II", len(vertices), len(triangles)))

    for vertex in vertices:
        stream.write(struct.pack("fff", vertex['x'], vertex['y'], vertex['z']))
    for triangle in triangles:
        indices = triangle['vertex_indices']
        assert len(indices) == 3
        stream.write(struct.pack(">III", *indices))

if __name__ == "__main__":
    model = read_ply(open(sys.argv[1]))
    write_mesh(open(sys.argv[2], "wb"), model)

### The End ###

