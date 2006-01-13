#!/usr/bin/env python

from Motion import AMC
from LinearAlgebra import inverse
import Numeric, sys

def interpolate (points):
    """Add frames to smooth the animation using cubic natural splines."""
    n = len (points)
    # A semi-arbitrary size limit for the chunk we're interpolating.
    if n > 10:
        mid = n / 2
        return interpolate (points[:mid]) + interpolate (points[mid-1:])

    # n is now the number of intervals over which we interpolate, not the number
    # of points.
    n -= 1
    # Create an list of all the intervals.
    intervals = []
    for i in range (n):
        t = 6 * i
        intervals.append (((t, points[i]), (t + 6, points[i + 1])))

    # Create the matrices
    s = n * 4
    A = Numeric.zeros ((s, s))
    b = Numeric.zeros ((s, 1))

    # FIXME - Filling the matrices could probably be optimized more...

    row = 0
    # For each interval, constrain the cubic approximation to equal the
    # endpoints at the endpoints.
    for i in range (n):
        col = 4 * i
        t, y = intervals[i][0]
        A[row, col:col+4] = [1, t, t**2, t**3]
        b[row] = y
        row += 1

        t, y = intervals[i][1]
        A[row, col:col+4] = [1, t, t**2, t**3]
        b[row] = y
        row += 1

    # Except for the end points, constrain the first and second derivatives of
    # each point to be equal across intervals.
    for i in range (n):
        col = 4 * i
        t, y = intervals[i][1]
        A[row, col:col+8] = [0, 1, 2*t, 3*t**2, 0, -1, -2*t, -3*t**2]
        row += 1
        A[row, col:col+8] = [0, 0, 2, 6*t, 0, 0, -2, -6*t]
        row += 1

    # Constrain the second derivatives of the endpoints to be 0.
    t, y = intervals[0][0]
    A[row, :4] = [0, 0, 2, 6*t]
    row += 1
    t, y = intervals[-1][1]
    A[row, -4:] = [0, 0, 2, 6*t]

    # Solve for the coefficients.
    Ainv = inverse (A)
    z    = Numeric.matrixmultiply (Ainv, b)

    # Fill in 5 points between each point we have now to smooth the sequence.
    # FIXME - Implement
    smoothed = []
    for i in range (n):
        t0, y0 = intervals[i][0]
        t1, y1 = intervals[i][1]

        smoothed.append (y0)
        a1, a2, a3, a4 = z[i * 4:i * 4 + 4, 0]
        for j in range (t0, t1):
            smoothed.append (a1 + a2*j + a3*j**2 + a4*j**3)

    # Append the last value
    smoothed.append (intervals[-1][1][1])

    return smoothed


if len (sys.argv) != 3:
    print "usage:", sys.argv[0], "<input AMC file> <output AMC file>"
    sys.exit (1)

amc = AMC.from_file (sys.argv[1])
interpolated = AMC ()
interpolated.format = amc.format

for bone, data in amc.bones.iteritems():
    temp = []
    # Iterate across the rows of the transposed matrix. We have to transpose
    # because we want a list of the angles in one degree of freedom across all
    # of the frames, but the data is stored with the rows being the angles for
    # all degrees of freedom across only one frame.
    for row in Numeric.transpose (data): temp.append (interpolate (row))
    # Create a new matrix from the smoothed data and transpose it again so that
    # the rows and columsn are frames and degrees of freedom, respectively.
    interpolated.bones[bone] = Numeric.transpose (Numeric.array (temp))

# interpolated.save (sys.argv[2])

# vim: ts=4:sw=4:et
