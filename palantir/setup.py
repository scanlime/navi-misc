#!/usr/bin/env python
''' setup.py

Installs Palantir.
'''

from distutils.core import setup

setup(name = 'palantir',
      version = '0.1',
      description = 'RPG IRC client',
      author = 'Evan Sheehan',
      author_email = 'evan@navi.cx',
      url = 'http://palantir.navi.cx',
      packages = ['Common', 'GtkPalantir'],
      scripts = ['palantir-client.py', 'palantir-sheets'],
      data_files = [('palantir/data',['data/palantirMain.glade']),
                    ('palantir/pixmaps',['data/pixmaps/dm.png',
		                             'data/pixmaps/stock_exit.png',
                                 'data/pixmaps/palantir-icon.png',
		                             'data/pixmaps/palantir-logo-small.png']),
		    ('palantir/sheets/layouts',['data/charactersheets/sheets/sheet.xml'])]
      )
