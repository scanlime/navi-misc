#!/usr/bin/env python
from distutils.core import setup
from distutils.extension import Extension
from Pyrex.Distutils import build_ext
setup(
  name = "Flibble",
  version = "pre-0.10",
  description = "A simple python+pyrex graph vizualization package",
  maintainer = "Micah Dowty",
  maintainer_email = "micah@navi.cx",
  license = "LGPL",
  packages = ['Flibble'],
  ext_modules=[
    Extension("Flibble.ForceDirected", ["Flibble/ForceDirected.pyx"])
    ],
  cmdclass = {'build_ext': build_ext}
)
