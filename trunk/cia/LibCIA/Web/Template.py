""" LibCIA.Web.Template

Template classes for building web pages using our particular style
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

import Nouvelle.Twisted
from Nouvelle import tag, place


# Tags with 'class' attributes should be placed in this module
catalogList = tag('ul', _class="catalog")
headingTab = tag('a', _class="headingtab")
value = tag('strong')

class Section(Nouvelle.DocumentOwner):
    """A portion of the web page with a title and a body, that may be placed
       in any of the page's columns.
       """
    def render_title(self, context):
        return self.title

    def render_rows(self, context):
        return self.rows

    def render_body(self, context):
        # Wrap each of the rows returned by render_rows() in a proper <div> tag
        return [tag('div', _class="row")[r] for r in self.render_rows(context)]

    document = [
        tag('span', _class="section")[ place("title") ],
        tag('div', _class="section")[
            tag('div', _class="sectionTop")[" "],
            place("body"),
        ],
    ]


class StaticSection(Section):
    """A section containing static content, usable with tag-like syntax:
       StaticSection(title)[body]
       """
    def __init__(self, title, rows=[]):
        self.title = title
        self.rows = rows

    def __getitem__(self, rows):
        return self.__class__(self.title, [rows])


class Page(Nouvelle.Twisted.Page):
    """A template for pages using our CSS- all pages have a heading with
       title, subtitle, and site name. Pages may have a list of hyperlinked
       tabs at the bottom of the heading, as well as columns containing
       Sections.
       """
    siteName = "CIA"
    subTitle = "More wet kittens than you can shake a cheese grater at"

    def render_pageTitle(self, context):
        return [self.render_mainTitle,
                ' - ',
                self.render_siteName]

    def render_siteName(self, context):
        return self.siteName

    def render_mainTitle(self, context):
        return self.mainTitle

    def render_subTitle(self, context):
        return self.subTitle

    def render_headingTabs(self, context):
        return self.headingTabs

    def render_leftColumn(self, context):
        return self.leftColumn

    def render_mainColumn(self, context):
        return self.mainColumn

    headingTabs = []
    leftColumn  = []
    mainColumn  = []

    document = tag('html')[
        tag('head')[
            tag('title')[ place("pageTitle") ],
            tag('style', type="text/css", media="all")[ "@import url(/style.css);" ],
            ],
        tag('body')[
            tag('div', _class="heading")[
                tag('div', _class="sitename")[ place("siteName") ],
                tag('div', _class="title")[ place("mainTitle") ],
                tag('div', _class="subtitle")[ place("subTitle") ],
                tag('div', _class="headingTabs")[ place("headingTabs") ],
            ],
            tag('table', _class="columns")[ tag('tr')[
                tag('td', _class="left")[ place("leftColumn") ],
                tag('td', _class="main")[ place("mainColumn") ],
            ]],
            tag('div', _class="footer")[
                tag('a', href="http://navi.cx")[
                    tag('img', src="/images/navi64.png", width="64", height="39", alt="Navi"),
                ],
            ],
        ],
    ]

### The End ###
