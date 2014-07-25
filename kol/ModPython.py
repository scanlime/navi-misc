''' ModPython.py

Module to facilitate the use of Nouvelle with mod_python apache
module. Page is the base class for creating webpages with Nouvelle
and mod_python. Each instance of Page is callable, by setting the document
attribute of a Page and calling that instance mod_python will create
the page.
'''
# Copyright (C) 2004 W. Evan Sheehan <evan@navi.cx>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import Nouvelle, string
from Nouvelle import tag

class Page:
  ''' Base class for using mod_python with Nouvelle. Creates callable instances
      that render the webpage when called.
      '''
  serializer = Nouvelle.Serializer

  def __call__(self):
    if hasattr(self, 'document'):
      doc = Page.serializer().render(self.document)
      return string.join(doc,'')
