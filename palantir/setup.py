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
      packages = ['Palantir', 'CharacterSheet'],
      scripts = ['palantir'],
      data_files = [('/usr/share/palantir/data',['data/palantirMain.glade']),
                    ('/usr/share/palantir/pixmaps',['data/pixmaps/dm.png',
		                             'data/pixmaps/stock_exit.png']),
		    ('/usr/share/pixmaps/',['data/pixmaps/palantir-icon.png',
		                            'data/pixmaps/palantir-logo-small.png']),
		    ('/usr/share/palantir/sheets/layouts',['CharacterSheet/layout/sheet.xml'])]
      )
