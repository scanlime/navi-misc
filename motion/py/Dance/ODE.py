''' ODE

ODE solvers and related classes.
'''

import Numeric, math


__all__ = ["RK4, ARK4"]


class Time:
    ''' A time object for use with ODE solvers. '''
    def __init__( self, initTime, iterations, stepsize ):
        ''' Start with a start time, time step and number of iterations. '''
        self.initial = initTime
        self.time = initTime
        self.n = iterations
        self.h = stepsize
        self.iteration = -1

    def __iter__( self ):
        ''' Make this object iterable. '''
        return self

    def next( self ):
        self.iteration += 1

        if self.iteration >= self.n: raise StopIteration

        self.time += self.h
        return self


class ODE:
    ''' Genereic ODE solver object. Just in case we end up using more than
        just RK4 (doubtful). All ODEs need a function, initial conditions,
        and some kind of time constraints.
        '''
    def __init__( self, func, ic, time, n, h ):
        self.f = func
        self.initial = ic
        self.time = Time( time, n, h )
        self.min = list (ic)
        self.max = list (ic)

    def __call__( self ):
        pass


class RK4( ODE ):
    ''' RK4 object. '''
    def __init__( self, func, ic, time=0, n=100000, h=.001 ):
        ODE.__init__( self, func, ic, time, n, h )

    def _step( self, last, i ):
        k1 = self.f( last, i.time, self.time.h )
        k2 = self.f( last + self.time.h/2. * k1,
                i.time + self.time.h/2., self.time.h )
        k3 = self.f( last + self.time.h/2. * k2,
                i.time + self.time.h/2., self.time.h )
        k4 = self.f( last + self.time.h * k3,
                i.time + self.time.h, self.time.h )

        return last + self.time.h/6.*(k1 + 2.*k2 + 2.*k3 + k4)

    def __call__( self, withTime=False ):
        if withTime:
            result = [(self.time.time, self.initial)]
            for i in self.time:
                step = self._step( result[i.iteration][1], i )
                #self._minmax( step )
                result.append( (self.time.time,step) )
        else:
            result = [self.initial]
            for i in self.time:
                step = self._step( result[i.iteration], i )
                #self._minmax( step )
                result.append( step )

        return result

    def _minmax( self, point ):
        x,y,z = point

        if x > self.max[0]: self.max[0] = x
        elif x < self.min[0]: self.min[0] = x

        if y > self.max[1]: self.max[1] = y
        elif y < self.min[1]: self.min[1] = y

        if z > self.max[2]: self.max[2] = z
        elif z < self.min[2]: self.min[2] = z


class ARK4( RK4 ):
    ''' An adaptive RK4 object. '''
    def __init__( self, func, ic, time=0, n=100000, h=.001, eps=.001 ):
        RK4.__init__( self, func, ic, time, n, h )
        # Error tolerance.
        self.epsilon = eps

    def _step( self, last, i ):
        # Take one step.
        single = RK4._step( self, last, i )

        # Take two half steps.
        checker = RK4( self.f, last, self.time.time, 2, .5*self.time.h )
        checker()

        # Find the difference.
        diff = checker.results[2] - single

        # If the difference is too large, decrease the time step.
        delta = math.sqrt( Numeric.dot( diff, diff ) )
        if delta > self.epsilon:
            self.time.h *= .5
            return self._step( last, i )
        # If the difference is less then our tolerance try doubling h.
        elif delta < self.epsilon:
            checker = RK4( self.f, last, self.time.time, 2, 2*self.time.h )
            checker()

            diff = checker.results[2] - single
            delta = math.sqrt( Numeric.dot( diff, diff ) )
            # Is it ok to double h?
            if delta < self.epsilon:
                self.time.h *= 2
                return checker.results[2]

        return single


### The End ###
