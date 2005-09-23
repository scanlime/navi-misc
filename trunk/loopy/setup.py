#!/usr/bin/env python
from distutils.core import setup
from distutils.extension import Extension
from distutils.sysconfig import get_python_version
setup(
    name = "Loopy",
    ext_modules=[ 
        Extension("loopy", ["loopy.c"],
                  libraries = ["python" + get_python_version()]),
    ],
)
