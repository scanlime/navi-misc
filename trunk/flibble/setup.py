#!/usr/bin/env python
from distutils.core import setup
from distutils.extension import Extension
from Pyrex.Distutils import build_ext
setup(
  name = "Flibble",
  ext_modules=[
    Extension("force_directed", ["force_directed.pyx"])
    ],
  cmdclass = {'build_ext': build_ext}
)
