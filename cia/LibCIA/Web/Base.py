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
from twisted.python import components
from nevow.iwoven import ISerializable
from nevow.tags import *


class Section:
    """Represents a named section which can be placed in any of the columns on a page.
       This is a base class with defaults that you probably want to override :)
       """
    def __init__(self, title, body):
        self.title = title
        self.body = body


class SectionSerializer(components.Adapter):
    __implements__ = ISerializable,

    def serialize(self, context, stream):
        return [
            span(_class="section")[ self.original.title ],
            div(_class="section")[
                div(_class="sectionTop")[" "],
                div(_class="row")[ self.original.body ],
            ]
        ]

components.registerAdapter(SectionSerializer, Section, ISerializable)


class Template(renderer.Renderer):
    """The template on which all other pages are built. This defines the overall
       structure of the page such that the CSS stays happy, with content generated
       by a set of functions easily overridden in subclasses
       """
    def render_pageTitle(self, context, data):
        """Return the <title> tag contents for this page- by default a combination
           of the main title and the site name.
           """
        return [
            self.render_mainTitle(context, data),
            " - ",
            self.render_siteName(context, data)
            ]

    def render_mainTitle(self, context, data):
        """Return the main title, this is displayed in large type in the heading"""
        return "Moose-o-Matic"

    def render_subTitle(self, context, data):
        """Return a subtitle to display below the main title"""
        return "More wet kittens than you can shake a cheese grater at"

    def render_siteName(self, context, data):
        """Return the site name, which is shown in the header and by default used
           to create the page title.
           """
        return "CIA"

    def render_headingTabs(self, context, data):
        return a(_class="headingTab", href="/")["CIA"]

    def render_leftColumn(self, context, data):
        return []

    def render_mainColumn(self, context, data):
        return []

    document = html[
        head[
            title[ directive("pageTitle") ],
            style(type="text/css", media="all")[ "@import url(/style.css);" ],
            ],
        body[
            div(_class="heading")[
                div(_class="sitename")[ directive("siteName") ],
                div(_class="title")[ directive("mainTitle") ],
                div(_class="subtitle")[ directive("subTitle") ],
                div(_class="headingTabs")[ directive("headingTabs") ],
            ],
            table(_class="columns")[ tr[
                td(_class="left")[ directive("leftColumn") ],
                td(_class="main")[ directive("mainColumn") ],
            ]],
        ],
    ]

### The End ###
