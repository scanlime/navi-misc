from django.conf import settings
from twisted.spread import pb
from twisted.internet import reactor
from twisted.python import failure
from cia.apps.api.util import json_result
import time


###########################
#    Bot Server Utils     #
###########################

def block(d):
    """Block on a deferred, resolving it into a value or an exception."""
    while not d.called:
        # This may only work correctly with selectreactor.
        # With the default timeout of zero, we'd be spinning
        # in a tight loop burning CPU while our I/O completes.
        # With 'None', our process will block in select().
        reactor.iterate(delay=None)
    if isinstance(d.result, failure.Failure):
        d.result.raiseException()
    return d.result

class BotServer:
    def __init__(self):
        self.factory = pb.PBClientFactory()
        reactor.connectUNIX(settings.CIA_BOT_SOCKET, self.factory)
        self.root = block(self.factory.getRootObject())

    def disconnect(self):
        self.factory.disconnect()

def needs_bot_server(view_func):
    def _wrapper(request, *args, **kwargs):
        server = BotServer()
        try:
            return view_func(request, server, *args, **kwargs)
        finally:
            server.disconnect()

    _wrapper.__doc__ = view_func.__doc__
    _wrapper.__dict__ = view_func.__dict__
    return _wrapper


###########################
#    Bots and Requests    #
###########################

def get_bot_dict(bot):
    """Return an info dictionary about a single IRC bot."""
    if bot:
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
    if request:
        return {
            'is_active': block(request.callRemote('active')),
            'is_fulfilled': block(request.callRemote('isFulfilled')),
            'user_count': block(request.callRemote('getUserCount')),
            'bots': map(get_bot_dict, block(request.callRemote('getBots'))),
            }

@json_result
@needs_bot_server
def request_status(request, server, host, channel, port=None):
    if port:
        port = int(port)
    request = block(server.root.callRemote(
        'findRequest', host, port, channel, create=False))
    return {
        'request': get_request_dict(request),
        }


###########################
#      Message Log        # 
###########################

@json_result
@needs_bot_server
def message_log(request, server):
    return {
        'log': block(server.root.callRemote('getMessageLog')),
        }
