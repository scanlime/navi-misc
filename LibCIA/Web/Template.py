""" LibCIA.Web.Template

Template classes for building web pages using our particular style
"""
#
# CIA open source notification system
# Copyright (C) 2003-2005 Micah Dowty <micah@navi.cx>
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

import Nouvelle
import Nouvelle.Twisted
from twisted.internet import defer
from Nouvelle import tag, place, xml, subcontext
import types, random

# Verify we have a new enough Nouvelle
if (not hasattr(Nouvelle, "version_info")) or Nouvelle.version_info < (0, 92, 1):
    raise Exception("The installed copy of Nouvelle is too old")

# Tag templates
catalogList = tag('ul', _class="catalog")
value = tag('strong')
error = tag('span', _class="error")
longError = tag('p', _class="error")
unableToFormat = error[ "Unable to format data" ]
breadcrumbSeparator = xml(" &raquo; ")
pageBody = tag('div', _class="pageBody")

# Stock images
fileIcon = tag('img', src="/images/bullet.png", _class="left-icon", width=9, height=9, alt="file")
dirIcon = tag('img', src="/images/folder.png", _class="left-icon", width=14, height=12, alt="directory")


def Photo(url, **attrs):
    """A factory for images presented as a photo"""
    return tag('div', _class='photo')[ tag('img', _class='photo', src=url, alt="Photo", **attrs) ]


def Bargraph(value, width=4, padding=0.2):
    """A factory for tags that use their size to express a value between 0 and 1"""
    return tag('span', _class='bargraph',
               style="padding: 0em %.4fem" % (value * width + padding))


def SubscriptionLink(url, content, icon="/images/rss.png", iconSize=(36,14)):
    """An anchor tag that can be used to link to RSS feeds.
       """
    return tag('a', href = url)[
              tag('img', src=icon, _class="left-icon", alt="RSS",
                  width=iconSize[0], height=iconSize[1]),
              content,
           ]


def SectionGrid(*rows):
    """Create a grid of sections, for layouts showing a lot of small boxes
       in a regular pattern.
       """
    return tag('table', _class="sectionGrid")[[
        tag('tr', _class="sectionGrid")[[
            tag('td', _class="sectionGrid")[
                cell
            ] for cell in row
        ]] for row in rows
    ]]


def MessageHeaders(d):
    """A factory for displaying message headers from a dictionary-like object.
       If order is important (it probably is) use twisted.python.util.OrderedDict.
       """
    return tag('table', _class="messageHeaders")[[
        tag('tr')[
            tag('td', _class='name')[ name, ":" ],
            tag('td', _class='value')[ value ],
        ]
        for name, value in d.iteritems()
    ]]


def randomlySubdivide(string, minLength, maxLength):
    """Randomly break a string into pieces not smaller than minLength
       or longer than maxLength.
       """
    parts = []
    while string:
        l = random.randint(minLength, maxLength)
        parts.append(string[:l])
        string = string[l:]
    return parts


def treeReplace(tree, a, b):
    """Replace any occurrance of 'a' with 'b' recursively in a tree of strings"""
    if type(tree) in (list, tuple):
        return [treeReplace(i, a, b) for i in tree]
    elif type(tree) in (str, unicode):
        return tree.replace(a, b)
    else:
        return tree


class EmailLink:
    """This is a tag-like class for generating obfuscated links to email addresses,
       using the javascript-based technique described by Kieth Bell at:
          http://www.december14.net/ways/js/nospam.shtml
       """
    def __init__(self, href):
        self.href = href
        self.content = []

    def __getitem__(self, content):
        self.content = content
        return self

    def render(self, context):
        # Obfuscate the content
        content = treeReplace(self.content, "@", " at ")
        content = treeReplace(content, ".", " dot ")

        # Create an 'onmouseover' script that will replace our link's
        # href with the correct one. We start by randomly dividing the
        # email address into small chunks.
        parts = randomlySubdivide(self.href, 1, 5)
        script = "this.href=" + "+".join(["'%s'" % part for part in parts])

        # Assemble and render the final link
        return tag('a', href="/doc/mail", onmouseover=script)[ content ].render(context)


def FileTree(tree):
    """A factory for rendering file trees from a tree of nested dictionaries.
       Dictionaries with no children are treated as files, dictionaries with
       children are treated as directories.
       """
    keys = tree.keys()
    keys.sort()
    items = []
    for key in keys:
        if not key:
            # This can happen when there's a trailing slash, for example because a new directory
            # was added. (in clients that are smart enough to detect that) Ignore it here for now.
            continue

        if tree[key]:
            items.append( tag('li', _class='directory')[ key, FileTree(tree[key]) ])
        else:
            items.append( tag('li', _class='file')[ key ])

    return tag('ul', _class='fileTree')[ items ]


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
    reversedSortIndicator = tag('img', _class='sortIndicator', width=11, height=7,
                                src="/images/sort_up.png", alt="Reversed sort column")
    sortIndicator = tag('img', _class='sortIndicator', width=11, height=7,
                        src="/images/sort_down.png", alt="Sort column")

    def render_heading(self, context, column):
        # Disable resorting headings if we're serving a page to a web spider,
        # it will just mindlessly click on all the links, wasting time and polluting
        # its database with junk.
        if context['request'].isWebSpider():
            return column.render_heading(context)
        else:
            return Nouvelle.ResortableTable.render_heading(self, context, column)


class HideFromSpiders:
    """Hides its contents when isWebSpider is true. This can be used to show
       content that shouldn't be indexed by search engines if possible.
       """
    def __init__(self):
        self.content = []

    def __getitem__(self, content):
        self.content = content
        return self

    def render(self, context):
        if context['request'].isWebSpider():
            return []
        else:
            return self.content


class StaticSection(Section):
    """A section containing static content, usable with tag-like syntax:
       StaticSection(title)[body]
       """
    def __init__(self, title=None, rows=None):
        if rows is None:
            # This is important- remember that default values are only created
            # once and reused forever. A list specified as a default value would
            # be shared by all instances that don't specify their own list.
            rows = []
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


class SiteSearch(Section):
    """A renderable that provides a Google search of this site"""
    title = "site search"

    def __init__(self, width=20):
        self.width = width

    def render_rows(self, context):
        domain = context['request'].host[1]
        return [tag('form', method="get", action="http://www.google.com/search")[
                    tag('div')[
                        tag('img', src="/images/google32.png", width=70, height=32, alt="Google"), " ",
                        tag('input', _type='text', _name='q', size=self.width, maxlength=255, value=''),
                        tag('input', _type='hidden', _name='domains', value=domain),
                        tag('input', _type='hidden', _name='sitesearch', value=domain),
                    ]]]


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
    extraHeaders = []

    # Placeholders for site-specific customization
    site_aboveLeftColumn = []
    site_belowFirstSidebox = []
    site_belowLeftColumn = [
            SiteSearch(),
        ]
    site_hostingNotice = []
    site_topOfFooter = []
    site_mainServerNotice = tag('p', _class='smallprint')[
            "This is not the primary CIA server. It may be found at ",
            tag('a', href="http://cia.navi.cx")[ "cia.navi.cx" ], ".",
            tag('br'),
            ". Please report problems with this server to its administrator, "
            "rather than to the CIA project."
        ]
    site_middleOfFooter = []
    site_bottomOfFooter = []

    document = [
        xml('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" '
            '"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">\n'),

        tag('html', xmlns="http://www.w3.org/1999/xhtml")[
            tag('head')[
                tag('title')[ place("pageTitle") ],
                place('baseTag'),
		xml('<meta http-equiv="Content-Type" content="text/html;charset=UTF-8" />'),
                tag('link', rel='stylesheet', href='/default.css', type='text/css'),
                tag('link', rel='shortcut icon', href='/favicon.ico', type='image/png'),
                place('extraHeaders'),
                ],
            tag('body')[

                # Page heading, including title, subtitle, site name, and tabs
                tag('div', _class="heading")[
                    tag('table', _class="heading")[ tag('tr', _class="heading")[
                        tag('td', _class="title")[
                            tag('div', _class="mainTitle")[ place("mainTitle") ],
                            tag('div', _class="subTitle")[ place("subTitle") ],
                        ],
                        tag('td', _class="topRight")[
                            tag('a', _class="sitename", href="/")[ place("siteName") ]
                        ],
                    ]],
                    tag('div', _class="tabs")[ place("tabs") ],
                    tag('div', _class="tabBar")[ place("breadcrumbs") ],
                ],

                # The page body. We really shouldn't still be using a table for this...
                tag('table', _class="columns")[ tag('tr')[
                    tag('td', _class="left")[
                        place("templateLeftColumn"),
                    ],
                    tag('td', _class="main")[
                        place("mainColumn")
                    ],
                ]],

                tag('div', _class="footer")[

                    place("site_topOfFooter"),

                    # Yep, this should be valid XHTML
                    tag('a', href="http://validator.w3.org/check/referer")[
                        tag('img', src="/images/xhtml.png",
                            alt="Valid XHTML 1.1!", height=15, width=80, _class="footer"),
                    ],

                    xml(" "),

                    # This is a good place for a message about our hosting
                    place("site_hostingNotice"),

                    xml(" "),

                    # And valid CSS
                    tag('a', href="http://jigsaw.w3.org/css-validator/check/referer")[
                        tag('img', src="/images/css.png",
                            alt="Valid CSS!", height=15, width=80, _class="footer"),
                    ],

                    # By default include a notice that this isn't the "main" CIA server
                    place("site_mainServerNotice"),

                    place("site_middleOfFooter"),

                    # Legal goop
                    tag('p', _class='smallprint')[
                        xml("The CIA server is Copyright &copy; 2003-2005 "),
                        EmailLink('mailto:micah@navi.cx')["Micah Dowty"],
                        ", and released under the ",
                        tag('a', _href='/doc/COPYING')["GNU GPL"], ".", tag('br'),
                        "All hosted messages and metadata are owned by their respective authors.",
                    ],

                    # More optional text
                    place("site_bottomOfFooter"),

                ],
            ],
        ]]

    def render_pageTitle(self, context):
        # Wait for the title and site name to resolve into strings so we can mess with them a bit more
        result = defer.Deferred()
        defer.gatherResults([
            defer.maybeDeferred(self.render_mainTitle, context),
            defer.maybeDeferred(self.render_siteName, context),
            ]).addCallback(
            self._render_pageTitle, context, result
            ).addErrback(result.errback)
        return result

    def _render_pageTitle(self, titleAndSite, context, result):
        # Now that the title and site name have fully resolved, we can apply some heuristics...
        title, siteName = titleAndSite

        if type(title) in types.StringTypes and type(siteName) in types.StringTypes:
            # The title and site are plain strings. If it starts with or ends with the site name,
            # just use it as-is to avoid being overly redundant.
            if title == siteName or title.startswith(siteName + " ") or title.endswith(" " + siteName):
                result.callback(title)
                return

        # Otherwise, stick the title and site name together
        result.callback([title, ' - ', siteName])

    def render_templateLeftColumn(self, context):
        """A sneaky little rendering function that runs render_leftColumn,
           but then sticks in the site_* modifiers where possible. Note that
           this won't work if render_leftColumn returns a Deferred, but
           nothing in the CIA server does this yet.
           """
        boxes = (self.site_aboveLeftColumn +
                 self.render_leftColumn(context) +
                 self.site_belowLeftColumn)
        return [boxes[0]] + self.site_belowFirstSidebox + boxes[1:]

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

    def render_baseTag(self, context):
        """Return an HTML <base> tag pointing at this page's original URL.
           This keeps the page from breaking if it's saved to disk or copied elsewhere.
           """
        return tag('base', href = context['request'].prePathURL())

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
