""" LibCIA.Web.Keyring

An abstraction for managing capability keys sent by a web browser,
stored client-side in 'secure' HTTP cookies and sent to us thereafter
any time we have an HTTPS connection.

In addition to the low-level keyring abstraction, this module provides
a template object for showing login status and logging in.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micah@navi.cx>
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
from Nouvelle import tag
from urllib import quote


class Keyring:
    """The Keyring attaches to a Request, providing an interface for setting
       and querying capability keys stored in HTTP cookies.
       """
    def __init__(self, request):
        self.request = request

        # Parse our incoming cookies, these may be overridden by outgoing cookies
        cookie = request.getCookie("CIA-key-presence")
        if cookie and int(cookie):
            self.hasKey = True
        else:
            self.hasKey = False
        self.key = request.getCookie("CIA-key-value")

        # Are we setting the key just now?
        if 'CIA-set-key' in request.args:
            key = request.args['CIA-set-key'][0]
            if key:
                self.setKey(request.args['CIA-set-key'][0])
            else:
                self.unsetKey()

    def setKey(self, key, expires=None, presence=1):
        """Set the client's current key. This sets one insecure
           cookie to signal the key's presence, and another secure
           cookie with the key itself.

           Note that this should ONLY be used over an https connection.
           Normally the login form will use an https URL, so this should
           always be the case, however there's no way for us to tell,
           behind a proxy, whether we really have a secure connection
           or not until these cookies have been set.
           """
        self.request.addCookie("CIA-key-presence", presence, expires=expires, path='/',
                               comment=
                               "This cookie signals the presence of a CIA capability key "+
                               "but does not contain the key itself.")
        self.request.addCookie("CIA-key-value", key, expires=expires, path='/', secure=True,
                               comment=
                               "This cookie contains the CIA capability key you're logged in with. "+
                               "Its value should only be sent over secure channels.")
        self.key = key
        self.hasKey = presence

    def unsetKey(self):
        """Remove a key set with setKey()"""
        self.setKey("", presence=0)


def getKeyring(context):
    """Get the keyring associated with a request, creating it if necessary"""
    request = context['request']
    if hasattr(request, 'keyring'):
        return request.keyring
    else:
        request.keyring = Keyring(request)
        return request.keyring


def getSecureURL(context):
    """Return an HTTPS URL referring to the current page"""
    request = context['request']
    inet, addr, port = request.getHost()
    return quote('https://%s/%s' % (
        request.getRequestHostname(),
        "/".join(request.prepath)), ":/")


class SecuritySection(Template.Section):
    """A sidebar section that can be used to log in and show current key info"""
    title = 'security'

    def render_rows(self, context):
        keyring = getKeyring(context)
        rows = []

        if keyring.hasKey:
            rows.append(tag('strong')[ "Key set" ])
            actionText = "Replace key"
        else:
            rows.append("To modify metadata, you must have a key granting the "
                        "appropriate capabilities. If you have a key, enter it below "
                        "to begin editing metadata.")
            actionText = "Set key"

        rows.append(tag('form', method="post", action=getSecureURL(context))[
            tag('div')[ tag('input',  _type='password', _name='CIA-set-key', size=30) ],
            tag('div')[ tag('input',  _type='submit', value=actionText) ],
            ])
        return rows

### The End ###
