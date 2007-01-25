#!/usr/bin/env python
from distutils.core import setup
from distutils.extension import Extension
from distutils.sysconfig import get_python_version
setup(
    name = "fidtool",
    ext_modules=[ 
        Extension("_fidtool", ["_fidtool.c"], libraries=["png"]),
    ],
)
