#!/usr/bin/env python
"""Usage: selftest.py [options] [test] [...]

 Uses Python's "unittest" framework to perform a self-test on the rcpod.
 This also serves as an example for performing tests on other hardware
 with the rcpod and unittest.

 Pass a test suite name on the command line to run that test suite. Command
 line options provided by unittest can be listed with the "-h" flag.
 NOTE that most tests should NOT be run with external hardware connected
 to the rcpod. Using hardware other than what is described in the test
 descriptions below will produce incorrect test results and could damage
 the rcpod and/or the external hardware.

 Useful test sets provided by this module:

   safe    : All tests that can be run without setting any of the rcpod's
             I/O ports to output mode. This won't affect any hardware plugged
             into the rcpod, but can't test much.

   single  : Tests all API and hardware features possible with a single rcpod.
             This can verify API sanity in most cases, but only partially
             verifies hardware sanity.
             Note that this will modify the I/O port states, and it should NOT
             be run with any external hardware hooked up to the rcpod.

   dual    : Requires two rcpod units, with all I/O pins (excluding power,
             USB, and oscillator) connected to each other. Also runs a 'single'
             test on each rcpod first. This uses each rcpod to test the I/O
             features on the other.

 -- Micah Dowty <micah@picogui.org>
"""

import pyrcpod, unittest, sys


class safe(unittest.TestCase):

    def setUp(self):
        self.seq = range(10)

    def testFoo(self):
        self.assertEqual(5, 3)


if __name__ == '__main__':
    # If no arguments were given, unittest usually runs the default test
    # case. We don't set up a default test case, since we want the user
    # to have read the doc string before picking a test to run. Print
    # it and exit if there are no command line arguments.
    if len(sys.argv) > 1:
        unittest.main()
    else:
        print __doc__
