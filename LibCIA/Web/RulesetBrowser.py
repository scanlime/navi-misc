""" LibCIA.Web.RulesetBrowser

A web interface for CIA's ruleset database
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

import Template
import Nouvelle
from Nouvelle import tag, place
from LibCIA import Units
from twisted.protocols import http
from twisted.web import error
import urllib


class RulesetLink:
    """An anchor tag linking to the given ruleset URI"""
    def __init__(self, uri, tagFactory=tag('a')):
        self.uri = uri
        self.tagFactory = tagFactory

    def render(self, context):
        if self.uri.find("://") >= 0:
            scheme, hostAndPath = self.uri.split("://", 1)
        else:
            scheme = self.uri
            hostAndPath = ''
        linkUrl = context['rulesetsRootPath'] + urllib.quote(scheme) + '/' + urllib.quote(hostAndPath)
        return self.tagFactory(href=linkUrl)[self.uri]


class RulesetURIColumn(Nouvelle.Column):
    """A table column that displays a ruleset's URI"""
    heading = "URI"

    def getValue(self, ruleset):
        return ruleset.uri

    def render_data(self, context, ruleset):
        return RulesetLink(ruleset.uri)


class RulesetSchemeColumn(Nouvelle.Column):
    """A table column that displays a ruleset's URI scheme"""
    heading = "scheme"

    def getValue(self, ruleset):
        uri = ruleset.uri
        if uri.find("://"):
            return uri.split("://", 1)[0]


class RulesetSizeColumn(Nouvelle.Column):
    """A table column that shows a ruleset's size in bytes"""
    heading = "size"

    def __init__(self):
        self.formatter = Units.StorageUnits().format

    def getValue(self, ruleset):
        return len(str(ruleset))

    def render_data(self, context, item):
        return self.formatter(self.getValue(item))


class RulesetListSection(Template.Section):
    """Displays a list of URIs in a particular RulesetStorage"""
    title = "rulesets"

    columns = [
        RulesetURIColumn(),
        RulesetSchemeColumn(),
        RulesetSizeColumn(),
        ]

    def __init__(self, storage):
        # Extract the rulesets from the RulesetStorage's map of URIs to RulesetDelivery objects
        self.rulesets = [delivery.ruleset for delivery in storage.rulesetMap.itervalues()]

    def render_rows(self, context):
        return [Template.Table(self.rulesets, self.columns, id='ruleset')]


def singleRulesetPageFactory(storage, uri):
    """Create and return a SingleRulesetPage instance appropriate
       for the given parameters.
       """
    if uri in storage.rulesetMap:
        return SingleRulesetEditor(storage, uri)
    else:
        return Ruleset404(storage, uri)


class SingleRulesetPage(Template.Page):
    """A viewer/editor for one ruleset. This is a base class for all
       pages that refer to a single ruleset.
       """
    subTitle = "The CIA ruleset editor, better than a fusion powered duck"

    def __init__(self, storage, uri):
        self.storage = storage
        self.uri = uri

    def getChildWithDefault(self, name, request):
        """The first child of RulesetPage adds the URI scheme, the rest
           add to the URI host and path.
           """
        if self.uri.find("://") < 0:
            newUri = self.uri + '://' + name
        else:
            newUri = self.uri + '/' + name
        return singleRulesetPageFactory(self.storage, newUri)

    def render_mainTitle(self, context):
        return self.uri


class RulesetEditorSection(Template.Section):
    """A Section that can view and edit one ruleset"""
    def __init__(self, storage, uri):
        self.storage = storage
        self.uri = uri

    def render_ruleset(self, context):
        return tag('pre')[self.storage.rulesetMap[self.uri].ruleset]

    rows = [ place('ruleset') ]
    title = "ruleset"


class RulesetTotalsSection(Template.Section):
    """A Section that displays fun-filled facts about our ruleset database"""
    def __init__(self, storage):
        self.total = 0
        self.schemeTotals = {}
        self.ircServers = {}
        for uri in storage.rulesetMap.keys():
            self.total += 1
            if uri.find("://") >= 0:
                scheme, hostAndPath = uri.split("://", 1)
                self.schemeTotals[scheme] = self.schemeTotals.get(scheme, 0) + 1
                if scheme == "irc":
                    host = hostAndPath.split("/", 1)[0]
                    self.ircServers[host] = True

    def render_schemeTotal(self, context, scheme):
        return self.schemeTotals.get(scheme, 0)

    def render_ircServers(self, context):
        return len(self.ircServers)

    title = "totals"
    rows = [[
        Template.value[ place('total') ],
        ' total rulesets are registered. This includes ',
        Template.value[ place('schemeTotal', 'irc') ],
        ' IRC channels on ',
        Template.value[ place('ircServers') ],
        ' servers, and ',
        Template.value[ place('schemeTotal', 'stats') ],
        ' stats rulesets.',
        ]]


class SingleRulesetEditor(SingleRulesetPage):
    """A page that can view and edit one ruleset"""
    def render_mainColumn(self, context):
        return [
            RulesetEditorSection(self.storage, self.uri)
            ]


class Ruleset404(SingleRulesetPage):
    """A page for rulesets that don't exist"""
    def render(self, request):
        return error.NoResource("No ruleset for %r" % self.uri).render(request)


class RulesetList(Template.Page):
    """A web page listing all available rulesets. Children
       of this page are URISchemePage instances, which
       have SingleRulesetPage instances as children. This
       lets URLs like /rulesets/irc/irc.freenode.net/commits work.
       """
    def __init__(self, storage):
        self.storage = storage

    def findRootPath(self, request):
        """Find the URL path referring to the root of our ruleset browser.
           The returned path always ends in a slash.
           Since this function is only present in the root page,
           this is easy :)
           """
        path = request.path
        if path and path[-1] != '/':
            path = path + '/'
        return path

    def preRender(self, context):
        context['rulesetsRootPath'] = self.findRootPath(context['request'])

    def getChildWithDefault(self, name, request):
        return singleRulesetPageFactory(self.storage, name)

    def render_mainColumn(self, context):
        return [
            RulesetListSection(self.storage),
            ]

    def render_leftColumn(self, context):
        return [
            RulesetTotalsSection(self.storage),
            ]

    mainTitle = "Ruleset List"
    subTitle = "the little tidbits of XML that make CIA work"

### The End ###
