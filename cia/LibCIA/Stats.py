""" LibCIA.Stats

Defines the stats:// URI for rulesets to target. The URI is of
the form stats://[optional/path/prefix]
The message passed to the URI from a ruleset is then URI encoded
and used as the rest of the stats:// path. This makes it easy to
create multiple namespaces for which stats are collected, and generate
the actual stats target using part of the message.
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

import Ruleset
import re, string, os


def urlencode(s, allowedChars = string.ascii_letters + string.digits + "-_"):
    """Return a URL-encoded version of 's', all characters not in the
       given list will be replaced with their hexadecimal value prefixed
       with '%'.
       """
    chars = []
    for char in s:
        if char in allowedChars:
            chars.append(char)
        else:
            chars.append("%%%02X" % ord(char))
    return "".join(chars)


class URIHandler(Ruleset.RegexURIHandler):
    """Handles stats:// URIs. The message passed to a stats:// URI is
       URI-encoded and added to the end of the stats:// URI to form
       a path identifying a class of messages that stats are collected for.
       """
    scheme = 'stats'
    regex = r"^stats://(?P<path>[a-zA-Z0-9_-]+(/[a-zA-Z0-9_-]+)*)$"

    def __init__(self, statsDirectory):
        self.statsDirectory = statsDirectory
        Ruleset.RegexURIHandler.__init__(self)

    def message(self, uri, message, content):
        # Stick the URI's path and the content together into a stats
        # path, URL-encoding the content first.
        path = self.parseURI(uri)['path'] + "/" + urlencode(content)

        # Form a complete path to the stats directory in question,
        # and increment its counters and such.
        incrementStats(os.path.join(self.statsDirectory, path))


def incrementStats(path):
    """Increment the stats stored at the given path, creating it if necessary"""
    try:
        os.path.makedirs(path)
    except OSError:
        # Directory probably already exists
        pass


### The End ###
