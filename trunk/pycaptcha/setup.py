#!/usr/bin/env python
from distutils.core import setup
setup (name = "PyCAPTCHA",
       version = "0.2-pre",
       description = "A Python framework for CAPTCHA tests",
       maintainer = "Micah Dowty",
       maintainer_email = "micah@navi.cx",
       license = "LGPL",
       packages = [
    'Captcha',
    'Captcha.Visual',
    ],
       )

