#!/usr/bin/env python
#
# Little script to convert wavefront .obj files to several videoscape .obj files.
# Give the name of a wavefront .obj file on the command line. Several files of the
# form originalname_mesh.obj will be output.
#
import sys, re
filename = sys.argv[1]

basename = re.sub(r"\..*", "", filename)
infile = open(filename)

# Read in the model, creating a mesh dict associating names to lists of faces.
vertices = []
meshname = None
meshes = {}
for line in infile:
    if line[:2] == 'v ':
        vertices.append(map(float,line[2:].strip().split(" ")))
    elif line[:2] == 'g ':
        meshname = line[2:].strip()
    elif line[:2] == 'f ':
        vertexNumbers = [int(v.split("/")[0]) for v in line[2:].strip().split(" ")]
        vertexCoords = [vertices[n-1] for n in vertexNumbers]
        meshes.setdefault(meshname, []).append(vertexCoords)

# Export each mesh...
for (name, mesh) in meshes.iteritems():
    outfile = open("%s_%s.obj" % (basename, name), "w")
    
    # Make a new vertex list for just this mesh, with no duplicates
    currentVertexNumber = 0
    vertexNumbers = {}
    vertices = []
    for face in mesh:
        for vertex in face:
            if not vertexNumbers.has_key(tuple(vertex)):
                vertexNumbers[tuple(vertex)] = currentVertexNumber
                vertices.append(vertex)
                currentVertexNumber += 1

    # Write header
    outfile.write("3DG1\n%d\n" % currentVertexNumber)

    # Write vertices
    for vertex in vertices:
        outfile.write(" ".join(map(str, vertex)) + "\n")

    # Write faces
    for face in mesh:
        outfile.write("%d " % len(face))
        for vertex in face:
            outfile.write("%d " % vertexNumbers[tuple(vertex)])
        outfile.write("0xc0c0c000\n")
