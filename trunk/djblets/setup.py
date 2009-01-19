#!/usr/bin/env python
#
# setup.py -- Installation for djblets
#
# Copyright (C) 2008 Christian Hammond
# Copyright (C) 2007-2008 David Trowbridge
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

from ez_setup import use_setuptools
use_setuptools()

from setuptools import setup, find_packages

from setuptools.command.test import test


def run_tests(*args):
    import os
    os.system("tests/runtests.py")

test.run_tests = run_tests


VERSION = "0.5alpha1"
IS_RELEASE = True


if IS_RELEASE:
    download_url = "http://www.review-board.org/downloads/releases/"
else:
    download_url = "http://www.review-board.org/downloads/nightlies/"


setup(name="Djblets",
      version=VERSION,
      test_suite="dummy",
      license="MIT",
      description="A collection of useful classes and functions for Django",
      packages=find_packages(),
      install_requires=['Django>=1.0.2', 'PIL'],
      dependency_links = [
          "http://www.review-board.org/downloads/mirror/",
          download_url,
      ],
      include_package_data=True,
      zip_safe=False,
      maintainer="Christian Hammond",
      maintainer_email="chipx86@chipx86.com",
      url="http://www.review-board.org/wiki/Djblets",
      download_url=download_url,
      classifiers=[
          "Development Status :: 3 - Alpha",
          "Environment :: Web Environment",
          "Framework :: Django",
          "Intended Audience :: Developers",
          "License :: OSI Approved :: MIT License",
          "Operating System :: OS Independent",
          "Programming Language :: Python",
          "Topic :: Software Development",
          "Topic :: Software Development :: Libraries :: Python Modules",
      ]
)
