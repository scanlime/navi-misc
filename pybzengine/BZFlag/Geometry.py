""" BZFlag.Geometry

Geometry utilities based on the vector operations provided
by the Numeric module.
"""
#
# Python BZFlag Package
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from __future__ import division
from Numeric import *
from LinearAlgebra import *
import math

__all__ = ['cross', 'magnitude2', 'magnitude', 'normalize', 'vectorAngle',
           'unitVectorAngle', 'autoVertexNormals', 'pointGrid']


def cross(u, v, result=None):
    """Perform a cross product of two vectors or n-dimensional arrays of vectors"""
    u = asarray(u)
    v = asarray(v)
    if result is None:
        result = zeros(u.shape, u.typecode())
    ux = u[...,0]
    uy = u[...,1]
    uz = u[...,2]
    vx = v[...,0]
    vy = v[...,1]
    vz = v[...,2]

    #     [ i  j  k  ]
    # det [ ux uy uz ]
    #     [ vx vy vz ]
    result[...,0] = vz*uy - uz*vy
    result[...,1] = uz*vx - vz*ux
    result[...,2] = ux*vy - vx*uy
    return result


def magnitude2(a):
    """Calculate the squared magnitude of a vector or each vector in an n-dimensional array"""
    a = asarray(a)
    return sum(a*a, -1)


def magnitude(a):
    """Calculate the magnitude of a vector or each vector in an n-dimensional array"""
    a = asarray(a)
    result = magnitude2(a)
    if type(result) == ArrayType:
        sqrt(result, result)
        return result
    else:
        return sqrt(result)


def normalize(a, result=None):
    """Normalize a vector or each vector in an n-dimensional array of vectors"""
    a = asarray(a)
    try:
        if len(a.shape) > 1:
            if result is None:
                result = zeros(a.shape, a.typecode())
            divide(a, magnitude(a)[..., NewAxis], result)
            return result
        else:
            return a / magnitude(a)
    except:
        return zeros(a.shape, a.typecode())


def vectorAngle(a,b):
    """Find the angle between vectors, in degrees"""
    a = asarray(a)
    b = asarray(b)
    try:
        return arccos(dot(a,b) / (magnitude(a) * magnitude(b))) * 180 / math.pi
    except ValueError:
        # arccos of equal vectors explodes
        return 0


def unitVectorAngle(a,b):
    """Same as vectorAngle, but assumes that a and b are unit vectors"""
    a = asarray(a)
    b = asarray(b)
    try:
        return math.acos(dot(a,b)) * 180 / math.pi
    except ValueError:
        # acos of equal vectors explodes
        return 0


def autoVertexNormals(triangles, creaseAngle=60):
    """Automatically calculate vertex normals for the given triangle normals.
       Expects an array with a shape of the form (n, 3, 3), returns an array
       with the same shape.
       Polygons with an angle of less than creaseAngle betwen them are smooth shaded,
       others are flat shaded.
       """
    # Calculate normals for each triangle, repeated across all three vertices.
    # This makes all triangles initially flat shaded.
    a = triangles[:,0,:]
    b = triangles[:,1,:]
    c = triangles[:,2,:]
    triNormals = cross(b-a, c-a)
    normalize(triNormals, triNormals)
    normals = repeat(triNormals[:,NewAxis,...], 3, 1)

    # First we create a dictionary listing each use of a particular vertex.
    # This maps tuple(vertex) to a list of (triangleIndex, vertexIndex) tuples.
    uses = {}
    for i in xrange(len(triangles)):
        for vertex in xrange(3):
            uses.setdefault(tuple(triangles[i][vertex]), []).append((i, vertex))

    # Now we go back through our uses dict and find vertices to smooth by averaging
    for vertex, uses in uses.iteritems():
        # For every pair of uses...
        for useIndex1 in range(len(uses)):
            use1 = uses[useIndex1]
            for useIndex2 in range(useIndex1 + 1, len(uses)):
                use2 = uses[useIndex2]

                # Compare the angle between face normals, smoothing them if it's small enough
                if unitVectorAngle(triNormals[use1[0]], triNormals[use2[0]]) < creaseAngle:
                    normals[use1[0], use1[1]] += triNormals[use2[0]]
                    normals[use2[0], use2[1]] += triNormals[use1[0]]

    # Renormalize to finish the averaging between smoothed vertices
    normalize(normals, normals)
    return normals


def pointGrid(origin, vx, vy, resolution):
    """Create an array with the shape (resolution[0], resolution[1], len(origin)),
       of points filling the given cartesian space specified as an origin and two
       axes.

       For example, the pointGrid((-5,-5,0), (10,0,0), (0,0,10), (100,100)) would produce
       a 100x100 grid of points inside a 10x10 square centered on the origin in the XZ plane.

       Note that unlike the semantics of range() and arange(), all endpoints are included
       in the resulting array, rather than including all but the final value.
       """
    origin = asarray(origin)
    vx = asarray(vx)
    vy = asarray(vy)

    xaxis = reshape(repeat(arange(0.0, resolution[0]) / (resolution[0]-1), len(origin)), (-1, len(origin))) * vx
    yaxis = reshape(repeat(arange(0.0, resolution[1]) / (resolution[1]-1), len(origin)), (-1, len(origin))) * vy

    array = zeros((len(xaxis), len(yaxis), len(origin)), Float)
    array += origin
    array += repeat(xaxis[:,NewAxis,:], len(yaxis), 1)
    array += yaxis
    return array

### The End ###
