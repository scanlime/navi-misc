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
from Nouvelle import tag, place, xml, subcontext


# Tag templates
catalogList = tag('ul', _class="catalog")
value = tag('strong')
error = tag('span', _class="error")
unableToFormat = error[ "Unable to format data" ]
breadcrumbSeparator = xml(" &raquo; ")


def Photo(url, **attrs):
    """A factory for images presented as a photo"""
    return tag('div', _class='photo')[ tag('img', _class='photo', src=url, **attrs) ]

def Bargraph(value, width=4, padding=0.2):
    """A factory for tags that use their size to express a value between 0 and 1"""
    return tag('span', _class='bargraph',
               style="padding: 0em %.4fem" % (value * width + padding))


class Section:
    """A renderable portion of the web page with a title and a body,
       that may be placed in any of the page's columns.

       A section can be made up of any number of 'rows', which manifest
       themselves as whitespace-padded sections. A section without any
       rows is completely hidden.
       """
    def render_title(self, context):
        return self.title

    def render_rows(self, context):
        return self.rows

    def render(self, context):
        result = defer.Deferred()
        defer.maybeDeferred(self.render_rows, context).addCallback(
            self._render, result).addErrback(result.errback)
        # Optimize out Deferreds where we can
        if result.called:
            return result.result
        return result

    def _render(self, rows, result):
        """The backend for render(), called once our rows list is known"""
        if rows:
            result.callback(subcontext(owner=self)[
                tag('span', _class="section")[ place("title") ],
                tag('div', _class="section")[
                    tag('div', _class="sectionTop")[" "],
                    [tag('div', _class="row")[r] for r in rows],
                ],
            ])
        else:
            result.callback([])


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
    def __init__(self, title=None, rows=[]):
        self.title = title
        self.rows = rows

    def __call__(self, title=None, rows=None):
        n = StaticSection()
        if title is None:
            n.title = self.title
        else:
            n.title = title
        if rows is None:
            n.rows = list(self.rows)
        else:
            n.rows = rows
        return n

    def __getitem__(self, rows):
        return self.__class__(self.title, [rows])


class Page(Nouvelle.Twisted.Page):
    """A template for pages using our CSS- all pages have a heading with
       title, subtitle, and site name. The content of each page is in
       columns holding sections, while each page shares certain navigation
       features- section tabs at the top, and a 'breadcrumbs' display
       linking to and showing a page's parent pages.
       """
    siteName = "CIA"
    mainTitle = None
    subTitle = []
    leftColumn  = []
    mainColumn  = []

    document = [
        xml('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" '
            '"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">\n'),

        tag('html', xmlns="http://www.w3.org/1999/xhtml")[
            tag('head')[
                tag('title')[ place("pageTitle") ],
                tag('style', type="text/css", media="all")[ "@import url(/style.css);" ],
                ],
            tag('body')[
                tag('div', _class="heading")[
                    tag('table', _class="heading")[ tag('tr', _class="heading")[
                        tag('td', _class="title")[
                            tag('div', _class="mainTitle")[ place("mainTitle") ],
                            tag('div', _class="subTitle")[ place("subTitle") ],
                        ],
                        tag('td', _class="sitename")[
                            tag('a', _class="sitename", href="/")[ place("siteName") ],
                        ],
                    ]],
                    tag('div', _class="tabs")[ place("tabs") ],
                    tag('div', _class="tabBar")[ place("breadcrumbs") ],
                ],
                tag('table', _class="columns")[ tag('tr')[
                    tag('td', _class="left")[ place("leftColumn") ],
                    tag('td', _class="main")[ place("mainColumn") ],
                ]],
                tag('div', _class="footer")[

                    # Yep, this should be valid XHTML
                    tag('a', href="http://validator.w3.org/check/referer")[
                        tag('img', src="http://www.w3.org/Icons/valid-xhtml10",
                            alt="Valid XHTML 1.0!", height=31, width=88, _class="footer"),
                    ],

                    xml(" "),

                    # And valid CSS
                    tag('a', href="http://jigsaw.w3.org/css-validator/check/referer")[
                        tag('img', src="http://jigsaw.w3.org/css-validator/images/vcss",
                            alt="Valid CSS!", height=31, width=88, _class="footer"),
                    ],

                    # Legal goop
                    tag('p', _class='smallprint')[
                        "The CIA server and this web site are Copyright (C) 2003-2004 ",
                        tag('a', _href='mailto:micah@picogui.org')["Micah Dowty"],
                        ", and released under the ",
                        tag('a', _href='/doc/COPYING')["GNU GPL"], ".", tag('br'),
                        "All hosted messages and metadata are owned by their respective authors.",
                    ],

                ],
            ],
        ]]

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

    def render_leftColumn(self, context):
        return self.leftColumn

    def render_mainColumn(self, context):
        return self.mainColumn

    def render_breadcrumbs(self, context):
        places = [self.render_mainTitle(context)]
        node = self.parent()
        # If we don't at least have a parent node, breadcrumbs
        # are going to be pretty useless. Just stick in a
        # non-breaking space as a placeholder.
        if not node:
            return xml("&nbsp;")
        while node:
            places.insert(0, breadcrumbSeparator)
            places.insert(0, node.render_link(context))
            node = node.parent()
        return places

    def render_link(self, context):
        """Return a serializable object that should be used to link to this page.
           By default, this returns a plain link with the page's title, pointing
           to the page's URL.
           """
        return tag('a', href=self.getURL(context))[self.render_mainTitle(context)]

    def parent(self):
        """Pages must implement this to return their parent page.
           This is used for the default implementation of breadcrumbs.
           """
        pass

    def render_tabs(self, context):
        """The page's tabs show all named components"""
        tabs = []
        for component in context['request'].site.components:
            if component.name:

                # Allow the component to decide if it owns the current page
                if self in component:
                    id = 'active'
                else:
                    id = None

                # Some spacing between tabs. With CSS this won't have any effect,
                # but in non-CSS browsers it keeps the tabs from all appearing
                # as one squished together link.
                if tabs:
                    tabs.append(xml(" "))

                tabs.append(tag('a', _class='tab', id=id, href=component.url)[ component.name ])

        return tabs

    def getURL(self, context):
        """Retrieve a URL suitable for linking to this page."""
        pass

### The End ###
