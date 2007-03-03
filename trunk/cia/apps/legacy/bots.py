from django.conf import settings
from twisted.spread import pb
from twisted.internet import reactor
from twisted.python import failure
import time

class TimeoutError(Exception):
    pass

def block(d, timeout=5.0):
    """Block on a deferred, resolving it into a value or an exception.
       If the entire operation doesn't complete within a specified
       timeout, in seconds, this raises TimeoutError.
       """
    deadline = time.time() + timeout
    while not d.called:
        remaining = deadline - time.time()
        if remaining < 0:
            raise TimeoutError()
        reactor.iterate(delay = remaining)
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

