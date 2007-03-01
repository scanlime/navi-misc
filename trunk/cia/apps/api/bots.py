from cia.apps.api.util import json_result
from cia.apps.legacy.bots import block, needs_bot_server
import time


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
#   Server-wide Totals    #
###########################

@json_result
@needs_bot_server
def totals(request, server):
    return {
        'totals': block(server.root.callRemote('getTotals')),
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
