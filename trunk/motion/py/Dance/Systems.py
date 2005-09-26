''' Systems

These classes represent different systems we commonly use in
CSCI 4446 Chaotic Dynamics. All systems are callable with a
list of arguments and a time. They return a Numeric.array with 
the results when called.
'''

import Numeric


class System:
    ''' A superclass that effectively just specifies an interface
        for all systems.
        '''
    def __call__( self, args, time, step ):
        ''' All Systems should be callable. '''
        pass


class Lorenz( System ):
    ''' The Lorenz system. '''
    def __init__( self, a, r, b ):
        self.a = a
        self.r = r
        self.b = b

    def __call__( self, args, time, step ):
        x, y, z = args

        return Numeric.array( [self.a*(y - x),
                               self.r*x - y - x*z,
                               x*y - self.b*z] )


class Rossler( System ):
    ''' The Rossler system. '''
    def __init__( self, a, b, c, delta=False ):
        self.a = a
        self.b = b
        self.c = c
        self.deltaMatrix = delta

    def __call__( self, args, time, step ):
        x, y, z = args

        if self.deltaMatrix:
            matrix = [
             [self.a*(y-x), self.r*x-y-x*z, x*y-self.b*z],
             [-self.a*xx+self.a*xy, -self.a*yx+self.a*yy,
                -self.a*zx + self.a*zy],
             [xx*(self.r-z)-xy-x*xz, yx*(self.r-z)-yy-x*yz,
                zx*(self.r-z)-zy-x*zz],
             [y*xx+x*xy-self.b*xz, y*yx+x*yy-self.b*yz, y*zx+x*zy-self.b*zz]]
        else:
             matrix = [-(y + z),
                        x + self.a*y,
                        self.b + z*(x - self.c)]

        return Numeric.array( matrix )


# vim:ts=4:sw=4:et
