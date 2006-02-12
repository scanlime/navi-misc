#
# Interpolate.py: Classes for interpolating motion data.
#
# Copyright (C) 2005 W. Evan Sheehan
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

from LinearAlgebra import inverse
from Motion import AMC
import Numeric

def spline(data, quality):
    """Return an interpolated trajectory from data. The returned value will be
       either an AMC object or a Numeric array, depending on what type the
       original data is. quality is the number of interpolated points to insert
       between each point in the initial data.
       """
    # Special case: The input data is an AMC object. For each bone in the AMC
    # object create a spline. Return an AMC object.
    if data.__class__ == AMC:
        interpolated = AMC()
        for bone, motion in data.bones.iteritems():
            interpolated.bones[bone] = spline(motion, quality)

        return interpolated

    data = Numeric.array(data)
    length, dof = Numeric.shape(data)
    interpolated = Numeric.empty((length * quality, dof))

    # This is the range of times we'll be using for the vast majority of the
    # splining process.
    times = Numeric.arange(2, 3, 1. / quality)

    # This function is used to generate the intermediate points from the
    # constants and the time. 
    f = lambda c: lambda t: c[0] + c[1] * t + c[2] * t**2 + c[3] * t**3

    for frame in range(length - 3):
        # Generate matrices and solve for the constants for this section of the
        # data.
        A, b = _getMatrix(data[frame:frame + 4], dof)
        print A
        Ainv = inverse(A)
        z = [Numeric.matrixmultiply(Ainv, x) for x in b]

        for degree in dof:
            # Special case: At the beginning of the trajectory or the end we use
            # the beginning or end of the spline to interpolate. Normally we
            # only use the middle interval of the spline to interpolate.
            if frame == 0:
                interpolated[frame:frame + quality, degree] = map(f(z[dof][:4]), Numeric.arange(1, 2, 1. / quality))
            elif frame == length - 4:
                interpolated[frame, degree] = map(f(z[dof][-4:]), Numeric.arange(3, 4, 1. / quality))

            interpolated[frame, degree] = map(f(z[dof][4:8]), times)

    return interpolated

def _getMatrix(data, dof):
    """Create the matrices A and b for a spline with 4 data points."""    
    # We need to be sure that we've got exactly 4 data points. 
    assert(Numeric.shape(data)[0] == 4)

    A = Numeric.zeros((12, 12))
    b = [Numeric.zeros((12,)) for i in range(dof)]
    row = 0

    for t in range(1,4):
        # Constrain the spline to fit the 4 points in this chunk of the
        # trajectory.
        col = (t - 1) * 4
        A[row, col:col + 4] = [1, t, t**2, t**3]
        for i in range(dof): b[i][row] = data[t - 1, i]
        row += 1

        A[row, col:col + 4] = [1, t + 1, (t + 1)**2, (t + 1)**3]
        for i in range(dof): b[i][row] = data[t, i]
        row += 1

        # Constrain the first and second derivatives at each of the
        # internal points to be equal.
        if t < 3:
            A[row + 4, col:col + 8] = [0, 1, 2 * t, 3 * t**2, 0, -1, -2 * t, -3 * t**2]
            A[row + 5, col:col + 8] = [0, 0, 2, 6 * t, 0, 0, -2, -6 * t]

    # Constrain the second derivative of the end points of the trajectory to
    # be 0.
    A[-2, :4] = [0, 0, 2, 6]
    A[-1, -4:] = [0, 0, 2, 24]

    return (A, b)


# vim: ts=4:sw=4:et
