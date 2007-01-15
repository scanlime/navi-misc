from django.conf.urls.defaults import *
from cia.apps.api import bots, validator

# IRC Network names are like hostnames. They can be fully-qualified
# domain names, or they can be special local "hostnames" that refer to
# network definitions within the bot server.

network_name_re = r'(?P<host>([a-zA-Z0-9-]+\.)*[a-zA-Z]+)(:(?P<port>\d+))?'

# IRC channel names are loosely defined. The disallowed characters
# are specified as space, comma, and percent. Various control
# characters are disallowed- we disallow the whole range.
#
# Note that the slash ("/") *is* allowed in a channel name. We don't
# escape it, it's allowed as-is in our API URLs.

irc_channel_re = r'(?P<channel>[#&][^\s\x00-\x1f,%]+)'

urlpatterns = patterns('',
    (r'^irc-bot-requests/%s/%s/status/$' % (network_name_re, irc_channel_re), bots.request_status),
    (r'^irc-message-log/recent/$', bots.message_log),
    (r'^validators/ruleset/$', validator.ruleset),
)
