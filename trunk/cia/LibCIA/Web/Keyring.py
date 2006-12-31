""" LibCIA.Web.Keyring

An abstraction for managing capability keys sent by a web browser,
stored client-side in 'secure' HTTP cookies and sent to us thereafter
any time we have an HTTPS connection.

In addition to the low-level keyring abstraction, this module provides
a template object for showing login status and logging in.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2006 Micah Dowty <micah@navi.cx>
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

from twisted.internet import defer
import Template
from Nouvelle import tag
from urllib import quote
from LibCIA import Security


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
                # Are we setting it just for this session, or until further notice?
                if request.args.get('CIA-remember-me', (None,))[0]:
                    expires = "Sun, 31-Dec-2034 00:00:00 GMT"
                else:
                    expires = None

                self.setKey(request.args['CIA-set-key'][0], expires=expires)
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
        # The browser should take an expiration date in the past as a cue
        # to delete the cookie, but even if it doesn't presence=0 will make
        # it a no-op cookie.
        self.setKey("", presence=0, expires="Sun, 01-Jan-1990  00:00:00 GMT")


def getKeyring(context):
    """Get the keyring associated with a request, creating it if necessary"""
    request = context['request']
    if hasattr(request, 'keyring'):
        return request.keyring
    else:
        request.keyring = Keyring(request)
        return request.keyring


# rebuild()-friendly way of maintaining the current secure server...
if '_secureServer' not in globals():
    global _secureServer
    _secureServer = (None, None)

def setSecureServer(host=None, port=None):
    """Set the hostname and/or port used for secure connections"""
    global _secureServer
    _secureServer = (host or _secureServer[0], port or _secureServer[1])


def getSecureURL(context):
    """Return an HTTPS URL referring to the current page"""
    global _secureServer
    host, port = _secureServer
    request = context['request']
    if port:
        port = ":%d" % port
    else:
        port = ''
    if not host:
        host = request.getRequestHostname()
    return quote('https://%s%s/%s' % (
        host, port,
        "/".join(request.prepath)), ":/")


class SecuritySection(Template.Section):
    """A sidebar section that can be used to log in and show current key info"""
    title = 'security'

    def __init__(self, loginMessage="Enter your key below to log in:"):
        self.loginMessage = loginMessage

    def render_rows(self, context):
        keyring = getKeyring(context)

        if keyring.hasKey:
            # We have a key, list info about it and give a logout button
            return [
                "Logged in",
                self.makeCapabilityList(keyring.key),
                tag('form', method="post", action=getSecureURL(context))[
                    tag('div')[
                        tag('input', _type='hidden', _name='CIA-set-key', value=''),
                        tag('input', _type='submit', value="Log out"),
                    ],
                ]
            ]

        else:
            # We don't have a key, show a login box
            return [
                tag('form', method="post", action=getSecureURL(context))[
                    tag('p')[ self.loginMessage ],
                    tag('p')[ tag('input',  _type='password', _name='CIA-set-key', size=30) ],
                    tag('p')[
                        tag('input', _type='checkbox', _name='CIA-remember-me', value="1"),
                        " remember me",
                    ],
                    tag('p')[ tag('input', _type='submit', value="Set key") ],
                ]
            ]

    def makeCapabilityList(self, key):
        result = defer.Deferred()
        if key:
            Security.User(key=key).getCapabilities().addBoth(
               self._makeCapabilityList, result)
        else:
            result.callback([])
        return result

    def _makeCapabilityList(self, caps, result):
        if type(caps) is list:
            result.callback([
                "Available capabilities:",
                tag('ul')[[
                    tag('li')[ cap ] for cap in caps
                ]],
            ])
        else:
            result.callback([])

### The End ###
