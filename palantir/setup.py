#!/usr/bin/env python
''' setup.py

Installs Palantir.
'''

from distutils.core import setup

setup(name = 'palantir',
      version = '0.1-pre',
      description = 'RPG IRC client',
      author = 'Evan Sheehan',
      author_email = 'evan@navi.cx',
      url = 'http://palantir.navi.cx',
      license = "LGPL",
      packages = ['Common', 'GtkPalantir'],

      scripts = ['palantir-client.py', 'palantir-sheets'],

      data_files = [Data_Files(
        preserve_path = 1,
        base_dir      = 'install_lib',
        copy_to       = 'Common/data',
        strip_dirs    = 2,
        template      = ['graft data',])]
      )
