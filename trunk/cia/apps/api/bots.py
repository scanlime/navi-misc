from django.conf import settings
from django.http import Http404
from twisted.spread import pb
from twisted.internet import reactor
from twisted.python import failure
from cia.apps.api.util import json_result
import time


def block(d):
    """Block on a deferred, resolving it into a value or an exception."""
    while not d.called:
        reactor.iterate()
    if isinstance(d.result, failure.Failure):
        d.result.raiseException()
    return d.result

def getBotServer():
    factory = pb.PBClientFactory()
    reactor.connectUNIX(settings.CIA_BOT_SOCKET, factory)
    return block(factory.getRootObject())

def get_bot_dict(bot):
    """Return an info dictionary about a single IRC bot."""
    return {
        'nickname': block(bot.callRemote('getNickname')),
        'current_channels': block(bot.callRemote('getChannels')),
        'requested_channels': block(bot.callRemote('getRequestedChannels')),
        'network': block(bot.callRemote('getNetworkInfo')),
        'current_time': time.time(),
        'connect_time': block(bot.callRemote('getConnectTimestamp')),
        'inactive_time': block(bot.callRemote('getInactivity')),
        'is_full': block(bot.callRemote('isFull')),
        'lag': block(bot.callRemote('getLag')),
        }

def get_request_dict(request):
    """Return an info dictionary about a single bot request.
       This includes a 'bots' list, which includes an info
       dictionary for each bot on this request.
       """
    return {
        'is_active': block(request.callRemote('active')),
        'is_fulfilled': block(request.callRemote('isFulfilled')),
        'user_count': block(request.callRemote('getUserCount')),
        'bots': map(get_bot_dict, block(request.callRemote('getBots'))),
        }

@json_result
def request_status(request, host, channel, port=None):
    if port:
        port = int(port)
    request = block(getBotServer().callRemote(
        'findRequest', host, port, channel, create=False))
    if request:
        return get_request_dict(request)
    raise Http404
    

