""" LibCIA.Web.Stats.MessageViewer

An interface for viewing the individual messages stored by the stats subsystem
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

from twisted.web import resource, error
from LibCIA.Web import Template


class RootPage(resource.Resource):
    """A page that doesn't generate any interesting output, but whose children are message IDs"""
    def __init__(self, statsPage):
        self.statsPage = statsPage
        resource.Resource.__init__(self)

    def render(self, request):
        return error.NoResource("There's no index here, you need a message ID").render(request)

    def getChildWithDefault(self, name, request):
        if not name:
            # Ignore empty path sections
            return self
        else:
            return MessagePage(self.statsPage, int(name))


class MessagePage(Template.Page):
    """A page that views one message from the stats database"""
    isLeaf = 1

    def __init__(self, statsPage, id):
        self.statsPage = statsPage
        self.id = id

    def render_mainTitle(self, context):
        return [self.statsPage.render_mainTitle(context),
                " message #%d" % self.id]

### The End ###
