""" Captcha.Base

Base class for all types of CAPTCHA tests. All tests have one or
more solution, determined when the test is generated. Solutions
can be any python object,

All tests can be solved by presenting at least some preset number
of correct solutions. Some tests may only have one solution and require
one solution, but other tests may require N correct solutions of M
possible solutions.
"""
#
# PyCAPTCHA Package
# Copyright (C) 2004 Micah Dowty <micah@navi.cx>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import random, string, time

__all__ = ["BaseCaptcha", "Factory"]


def randomIdentifier(alphabet = string.ascii_letters + string.digits,
                     length = 24):
    return "".join([random.choice(alphabet) for i in xrange(length)])


class BaseCaptcha(object):
    """Base class for all CAPTCHA tests"""
    # Subclasses can override these to set the solution criteria
    minCorrectSolutions = 1
    maxIncorrectSolutions = 0

    def __init__(self):
        self.solutions = []

        # Each test has a unique identifier, used to refer to that test
        # later, and a creation time so it can expire later.
        self.id = randomIdentifier()
        self.creationTime = time.time()

    def addSolution(self, solution):
        self.solutions.append(solution)

    def testSolutions(self, solutions):
        """Test whether the given solutions are sufficient for this CAPTCHA"""
        numCorrect = 0
        numIncorrect = 0

        for solution in solutions:
            if solution in self.solutions:
                numCorrect += 1
            else:
                numIncorrect += 1

        return numCorrect >= self.minCorrectSolutions and \
               numIncorrect <= self.maxIncorrectSolutions


class Factory(object):
    """Creates BaseCaptcha instances on demand, and tests solutions.
       A given CAPTCHA can only be tested once. Success or not, we delete it.
       This prevents attacks where the answer to a CAPTCHA is repeatedly
       guessed. If a human gets the test wrong, they'll have to pick a different
       one to solve rather than getting the same one again.

       Unsolved CAPTCHAs expire after a given amount of time, given in seconds.
       The default is 1 hour.
       """
    def __init__(self, cls, lifetime=60*60):
        self.cls = cls
        self.lifetime = lifetime
        self.storedInstances = {}

    def new(self, *args, **kwargs):
        """Create a new instance of our assigned BaseCaptcha subclass, passing
           it any extra arguments we're given. This stores the result for
           later testing.
           """
        self.clean()
        inst = self.cls(*args, **kwargs)
        self.storedInstances[inst.id] = inst
        return inst

    def get(self, id):
        """Retrieve the CAPTCHA with the given ID. If it's expired already,
           this will return None. A typical web application will need to
           new() a CAPTCHA when generating an html page, then get() it later
           when its images or sounds must be rendered.
           """
        return self.storedInstances.get(id)

    def clean(self):
        """Removed expired tests"""
        expiredIds = []
        now = time.time()
        for inst in self.storedInstances.itervalues():
            if inst.creationTime + self.lifetime < now:
                expiredIds.append(inst.id)
        for id in expiredIds:
            del self.storedInstances[id]

    def test(self, id, solutions):
        """Test the given list of solutions against the BaseCaptcha instance
           created earlier with the given id. Returns True if the test passed,
           False on failure. In either case, the test is invalidated. Returns
           False in the case of an invalid id.
           """
        self.clean()
        inst = self.storedInstances.get(id)
        if not inst:
            return False
        result = inst.testSolutions(solutions)
        del self.storedInstances[id]
        return result

### The End ###
