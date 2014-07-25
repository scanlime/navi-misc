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

import sys,os
sys.path.append('/var/www/localhost/htdocs/kol')

import ModPython
from Nouvelle import tag

class Page(ModPython.Page):
  def __init__(self, document):
    self.document = tag('html')[document]

test = Page( tag('body') [
                tag('h1') [ 'header' ],
                tag('p') ['This is a test of the emergency broadcast system.']
              ]
            )

test2 = Page( tag('body') [
                tag('h3') ['header3'],
                tag('p') ['test #2']
              ]
             )
test3 = ModPython.Page()
