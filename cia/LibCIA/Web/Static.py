""" LibCIA.Static

Local subclasses and such related to serving static pages
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

from twisted.web import static


class File(static.File):
    """A subclass of the standard twisted.web.File that overrides
       the policy for adding slashes to the end of directory URLs.
       The default implementation of redirect() constructs a new URL
       from scratch, so it works quite badly when we're running through
       a proxy. VHostMonster only solves this if the proxy's original URLs are
       the same as our URLs except for the host and port.
       """
    def redirect(self, request):
        return redirectTo(request + '/', request)

### The End ###
