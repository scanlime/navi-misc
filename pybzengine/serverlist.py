#!/usr/bin/env python
from BZFlag import Event, Meta

loop = Event.EventLoop()

def callback(servers):
    for server in servers:
        print server.info()
    loop.stop()

Meta.MetaClient(loop).list(callback)
loop.run()


