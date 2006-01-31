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

    data         = Numeric.array(data)
    length, dof  = Numeric.shape(data)
    interpolated = Numeric.empty((length * quality, dof))

    # This is the range of times we'll be using for the vast majority of the
    # splining process.
    times = Numeric.arange(1, 2, 1. / quality)

    # This function is used to generate the intermediate points from the
    # constants and the time. 
    f = lambda c: lambda t: c[0] + c[1] * t + c[2] * t**2 + c[3] * t**3

    for frame in range(length - 3):
        # Generate matrices and solve for the constants for this section of the
        # data.
        # FIXME - I am wholly unconvinced that the nonsense about mapping arrays
        # to that lambda function actually works.
        A, b = __getMatrix(data[frame:frame + 4], dof)
        z    = Numeric.matrixmultiply(inverse(A), b)

        # Special case: At the beginning of the data we need to use the first
        # set of constants for the interpolation.
        if frame == 0:
            eqs = map(f, z[:4])
            for degree in range(dof):
                interpolated[:quality, degree] = map(eqs, Numeric.arange(0, 1, 1. / quality))

        # Interpolate the middle two points of this section of data.
        eqs = map(f, z[4:8])
        sFrame = (frame + 1) * quality
        for degree in range(dof):
            interpolated[sFrame:sFrame + quality, degree] = map(eqs, times)

    # When all is said and done, interpolate the last two pointsin the data.
    eqs = map(f, z[-4:])
    for degree in range(dof):
        interpolated[-quality:, degree] = map(eqs, Numeric.arange(2, 3, 1. / quality))

    return interpolated


def _getMatrix(data, dof):
    """Create the matrices A and b for a spline with 4 data points."""    
    # We need to be sure that we've got exactly 4 data points. 
    assert(Numeric.shape(data)[0] == 4)

    A   = Numeric.zeros((12, 12))
    b   = Numeric.zeros((12, dof))
    row = 0

    for t in range(3):
        # Constrain the spline to fit the 4 points in this chunk of the
        # trajectory.
        col = t * 4
        A[row, col:col + 4] = [1, t, t**2, t**3]
        b[row] = data[t]
        row += 1

        A[row, col:col + 4] = [1, t + 1, (t + 1)**2, (t + 1)**3]
        b[row] = data[t + 1]
        row += 1

        # Constrain the first and second derivatives at each of the
        # internal points to be equal.
        if t < 2:
            A[row + 4, col:col + 8] = [0, 1, 2 * t, 3 * t**2, 0, -1, -2 * t, -3 * t**2]
            A[row + 5, col:col + 8] = [0, 0, 2, 6 * t, 0, 0, -2, -6 * t]

    # Constrain the second derivative of the end points of the trajectory to
    # be 0.
    A[-2, :4]  = [0, 0, 2, 6 * t]
    A[-1, -4:] = [0, 0, 2, 6 * t]

    return (Numeric.resize(A, (12, 12, dof)), b)


# vim: ts=4:sw=4:et
