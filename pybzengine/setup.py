#!/usr/bin/env python
from distutils.core import setup
setup (name = "PyBZEngine",
       version = "0.1",
       description = "Simple 3D rendering engine and game framework",
       maintainer = "Micah Dowty",
       maintainer_email = "micah@picogui.org",
       license = "LGPL",
       packages = [
    'BZEngine',
    'BZEngine.UI',
    'BZEngine.UI.Drawable',
    'BZEngine.UI.Drawable.VRML',
    'BZEngine.UI.ThreeDRender',
    'BZEngine.UI.Viewport',
    'BZEngine.optik',
    ],
       )

