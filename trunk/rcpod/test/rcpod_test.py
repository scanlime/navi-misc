#
# Common code shared between the rcpod test scripts.
# Safe for 'from rcpod_test import *'
#
# -- Micah Dowty <micah@picogui.org>
#

__all__ = ['RcpodTestCase', 'main', 'testBytes', 'pyrcpod']

import unittest, sys, os

# Add this script's parent directory to the module search path, so that tests
# can be run from the source tree without having to have pyrcpod installed.
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
import pyrcpod


# A set of byte values used to test several things.
# This pattern includes all bits off, all bits on, the two
# 'checkerboard' bit patterns, and tests for MSB/LSB alignment.
testBytes = (0x00, 0xFF, 0xAA, 0x55, 0x01, 0x02, 0x80, 0x40)


class RcpodTestCase(unittest.TestCase):
    """A TestCase subclass that opens the first available
       rcpod device and closes it afterwards.
       """
    def setUp(self):
        self.rcpod = pyrcpod.devices[0].open()

    def tearDown(self):
        # Reset the rcpod before closing it, in case the test
        # left the I/Os in a funny state.
        self.rcpod.reset()
        self.rcpod.close()


def main():
    """A 'main' function for tests to invoke.
       Currently just passes control to unittest.main(),
       but this in the future may be used to specify rcpod-related
       command line options.
       """
    unittest.main()

### The End ###

