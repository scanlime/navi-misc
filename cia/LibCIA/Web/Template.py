""" LibCIA.Web.Template

Template classes for building web pages using our particular style
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import Nouvelle.Twisted
from twisted.internet import defer
from Nouvelle import tag, place, xml


# Tags with 'class' attributes should be placed in this module
catalogList = tag('ul', _class="catalog")
headingTab = tag('a', _class="headingtab")
value = tag('strong')


def Photo(url, **attrs):
    """A factory for images presented as a photo"""
    return tag('div', _class='photo')[ tag('img', _class='photo', src=url, **attrs) ]

def Bargraph(value, width=4, padding=0.2):
    """A factory for tags that use their size to express a value between 0 and 1"""
    return tag('span', _class='bargraph',
               style="padding: 0em %.4fem" % (value * width + padding))


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
        result = defer.Deferred()
        defer.maybeDeferred(self.render_rows, context).addCallback(
            self._assembleRows, result).addErrback(result.errback)
        if result.called:
            return result.result
        return result

    def _assembleRows(self, rows, result):
        result.callback([tag('div', _class="row")[r] for r in rows])

    document = [
        tag('span', _class="section")[ place("title") ],
        tag('div', _class="section")[
            tag('div', _class="sectionTop")[" "],
            place("body"),
        ],
    ]


class Table(Nouvelle.ResortableTable):
    """Add sorting indicators to Nouvelle's normal ResortableTable"""
    def render_heading(self, context, column):
        if self.columns[self.sortColumnIndex] == column:
            # This is the sort column, indicate the direction
            if self.sortReversed:
                indicator = tag('img', _class='sortIndicator', width=11, height=7,
                                src="/images/sort_up.png", alt="Reversed sort column")
            else:
                indicator = tag('img', _class='sortIndicator', width=11, height=7,
                                src="/images/sort_down.png", alt="Sort column")
        else:
            # Not the sort column
            indicator = []

        return [
            Nouvelle.ResortableTable.render_heading(self, context, column),
            indicator,
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

    document = [
        ## Commented out for now, as it seems to break some of the CSS formatting. Why?
        ##
        #xml('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" '
        #    '"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">\n'),

        tag('html', xmlns="http://www.w3.org/1999/xhtml")[
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
                        tag('img', _class="footer", src="/images/navi64.png", width="64", height="39", alt="Navi"),
                    ],
                ],
            ],
        ]]

### The End ###
