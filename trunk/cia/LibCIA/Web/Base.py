""" LibCIA.Web.Base

Base classes acting as document templates subclassed by other modules
in the web interface for CIA.
"""
#
# CIA open source notification system
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from nevow import renderer, stan
from nevow.tags import *


class Base(renderer.Renderer):
    """The template on which all other pages are built. This defines the overall
       structure of the page such that the CSS stays happy.
       """
    document = html[
        head[
            title[ "Squidgey" ],
            style(type="text/css", media="all")[ "@import url(/style.css);" ],
            ],
        body[
            div(_class="heading")[
                div(_class="sitename")["CIA"],
                div(_class="title")["Title-thingy"],
                div(_class="subtitle")["More wet kittens than you can shake a cheese grater at"],
                div(_class="headingTabs")[
                    a(_class="headingTab", href="/")["CIA"],
                ],
            ],
        ],
    ]

### The End ###
