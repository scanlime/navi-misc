#!/usr/bin/env python

from LinearAlgebra import inverse
import Motion, Numeric

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
    b = Numeric.zeros ((1, s))

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

    return points


# vim: ts=4:sw=4:et
