#!/usr/bin/env python
from distutils.core import setup, Extension
setup (name = "dc1394",
       version = "0.1",
       description = "A Python wrapper for libdc1394, a library to control ieee1394 digital cameras",
       maintainer = "Micah Dowty",
       maintainer_email = "micah@navi.cx",
       license = "LGPL",
       packages = ['dc1394'],
       ext_modules = [Extension('dc1394._libdc1394',
                                ['dc1394/libdc1394.i'],
                                libraries = ['dc1394_control', 'raw1394'])],
       )

