#!BPY
#
# Blender exporter for a binary '.mesh' format that is easy
# to load in OpenGL even when feeling particularly lazy.
# The format is binary, with floating point numbers stored
# in IEEE single-precision and integers in 32-bit big-endian.
#
# The header consists simply of two integers giving first the
# number of vertices then the number of triangles.
# These are followed by all vertices in GL_N3F_V3F format
# ready for glInterleavedArrays() then all triangles as groups
# of three integer vertex indices.
#
# Micah Dowty <micah@navi.cx>
#

"""
Name: 'Binary mesh...'
Blender: 234
Group: 'Export'
Tooltip: 'Save a simple binary .mesh file'
"""

import struct
from Blender import *

def newFilename(ext):
  return '.'.join(Get('filename').split('.')[:-1] + [ext])

def find_and_save_mesh(filename):
  for ob in Object.Get():
     if ob.getType() == 'Mesh':
        m = ob.getData()
        if len(m.verts) > 0:
          save_mesh(filename, m)
          return

def save_mesh(filename, mesh):
  file = open(filename, "w")

  # Write our header, giving the number of vertices and faces
  file.write(struct.pack(">II", len(mesh.verts), len(mesh.faces)))

  # Write interleaved vertex positions and normals
  for vertex in mesh.verts:
    file.write(struct.pack("fff", *vertex.no) +
               struct.pack("fff", *vertex.co))

  # Write vertex indices for all triangles
  for face in mesh.faces:
    assert len(face.v) == 3
    for vertex in face.v:
      file.write(struct.pack(">I", vertex.index))

Window.FileSelector(find_and_save_mesh, 'Export Binary Mesh', newFilename('mesh'))

### The End ###
