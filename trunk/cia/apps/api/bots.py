from cia.apps.api.util import json_result
from cia.apps.legacy.bots import block, needs_bot_server
import time


###########################
#    Bots and Requests    #
###########################

@json_result
@needs_bot_server
def request_status(request, server, host, channel, port=None):
    return {
        'request': block(server.root.callRemote('findRequestInfo', host, int(port or 0) or None, channel))
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
