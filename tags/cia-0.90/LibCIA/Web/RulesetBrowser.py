""" LibCIA.Web.RulesetBrowser

A web interface for CIA's ruleset database
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

import Template, Server
import Nouvelle
from Nouvelle import tag, place
from LibCIA import Units
from twisted.protocols import http
from twisted.web import error
import urllib, posixpath


class Component(Server.Component):
    """A Server component for our ruleset browser"""
    name = 'Rulesets'

    def __init__(self, storage):
        self.storage = storage
        self.resource = RulesetList(self)

    def __contains__(self, page):
        return isinstance(page, RulesetList) or isinstance(page, RulesetEditorPage)


class RulesetURIColumn(Nouvelle.Column):
    """A table column that displays a ruleset's URI"""
    heading = "URI"

    def getValue(self, ruleset):
        return ruleset.uri

    def render_data(self, context, ruleset):
        return RulesetEditorPage(context['component'], ruleset.uri).render_link(context)


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

    def __init__(self, component):
        # Extract the rulesets from the RulesetStorage's map of URIs to RulesetDelivery objects
        self.rulesets = [delivery.ruleset for delivery in component.storage.rulesetMap.itervalues()]

    def render_rows(self, context):
        return [Template.Table(self.rulesets, self.columns, id='ruleset')]


class RulesetEditorPage(Template.Page):
    """A viewer/editor for one ruleset. This is a base class for all
       pages that refer to a single ruleset.
       """
    subTitle = "The CIA ruleset editor, better than a fusion powered duck"

    def __init__(self, component, uri):
        self.component = component
        self.uri = uri

    def parent(self):
        return RulesetList(self.component)

    def render(self, request):
        if self.uri not in self.component.storage.rulesetMap:
            return error.NoResource("No ruleset for %r" % self.uri).render(request)
        return Template.Page.render(self, request)

    def getChildWithDefault(self, name, request):
        """The first child of RulesetPage adds the URI scheme, the rest
           add to the URI host and path.
           """
        if self.uri.find("://") < 0:
            newUri = self.uri + '://' + name
        else:
            newUri = self.uri + '/' + name
        return self.__class__(self.component, newUri)

    def render_mainTitle(self, context):
        return self.uri

    def getURL(self, context):
        if self.uri.find("://") >= 0:
            scheme, hostAndPath = self.uri.split("://", 1)
        else:
            scheme = self.uri
            hostAndPath = ''
        return posixpath.join(self.component.url, urllib.quote(scheme), urllib.quote(hostAndPath))

    def render_mainColumn(self, context):
        return [
            RulesetEditorSection(self.component, self.uri)
            ]


class RulesetEditorSection(Template.Section):
    """A Section that can view and edit one ruleset"""
    def __init__(self, component, uri):
        self.component = component
        self.uri = uri

    def render_ruleset(self, context):
        return tag('pre')[self.component.storage.rulesetMap[self.uri].ruleset]

    rows = [ place('ruleset') ]
    title = "ruleset"


class RulesetTotalsSection(Template.Section):
    """A Section that displays fun-filled facts about our ruleset database"""
    def __init__(self, component):
        self.total = 0
        self.schemeTotals = {}
        self.ircServers = {}
        for uri in component.storage.rulesetMap.keys():
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


class RulesetList(Template.Page):
    """A web page listing all available rulesets. Children
       of this page are URISchemePage instances, which
       have RulesetEditorPage instances as children. This
       lets URLs like /rulesets/irc/irc.freenode.net/commits work.
       """
    mainTitle = "Ruleset List"
    subTitle = "the little tidbits of XML that make CIA work"

    def __init__(self, component):
        self.component = component

    def preRender(self, context):
        context['component'] = self.component

    def getChildWithDefault(self, name, request):
        return RulesetEditorPage(self.component, name)

    def render_mainColumn(self, context):
        return [
            RulesetListSection(self.component),
            ]

    def render_leftColumn(self, context):
        return [
            RulesetTotalsSection(self.component),
            ]

    def getURL(self, context):
        return self.component.url

### The End ###
