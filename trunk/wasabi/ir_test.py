#!/usr/bin/env python

from twisted.internet import reactor, protocol
from Wasabi.Hardware import IR

factory = protocol.ClientFactory()
factory.protocol = IR.CodeReceiver

reactor.connectUNIX("/dev/lircd", factory)
reactor.run()
