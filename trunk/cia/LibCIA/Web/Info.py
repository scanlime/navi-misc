""" LibCIA.Web.Info

Just a cute little page with informational doodads on it.
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

from LibCIA.Web import Template
from LibCIA import TimeUtil, XML
from Nouvelle import place, tag
import LibCIA
import time, sys


class Clock(Template.Section):
    title = 'UTC clock'

    def render_rows(self, context):
        return [TimeUtil.formatDate(time.time())]


class Version(Template.Section):
    title = 'version'

    def render_rows(self, context):
        rows = [LibCIA.__version__]
        svnRev = self.getSvnRevision()
        if svnRev:
            rows.append("Revision %s" % svnRev)
        return rows

    def getSvnRevision(self):
        """Return the current Subversion repository revision, or None
           if we're not in an svn working copy or it can't be parsed.
           """
        try:
            entries = XML.parseString(open(".svn/entries").read())
            highestRev = 0
            for tag in entries.elements():
                if tag.name == 'entry':
                    rev = tag.getAttribute('committed-rev', 0)
                    if rev > highestRev:
                        highestRev = rev
            return highestRev
        except:
            return None


class WebServer(Template.Section):
    title = 'web server'

    def render_requestCount(self, context):
        return context['request'].site.requestCount

    def render_uptime(self, context):
        return TimeUtil.formatDuration(time.time() - context['request'].site.serverStartTime)

    rows = [
               [
                   Template.value[ place('requestCount') ],
                   ' requests processed so far',
               ],
               [
                   'The server has been up for ',
                   Template.value[ place('uptime') ],
               ],
           ]


class System(Template.Section):
    title = 'system'

    rows = [
               [tag('pre')[ 'Python %s on %s' % (sys.version, sys.platform) ]],
           ]


class Page(Template.Page):
    """A web page showing information about the server"""
    mainTitle = 'Server Info'

    leftColumn = [
        Version(),
        Clock(),
        ]

    mainColumn = [
        WebServer(),
        System(),
        ]

### The End ###
