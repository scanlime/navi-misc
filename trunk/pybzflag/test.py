#!/usr/bin/env python

import BZFlag.Network
import BZFlag.Protocol.FromServer
import BZFlag.ListServer

# Just pick the first server on the list that's compatible with us
server = BZFlag.ListServer.getDefault().filteredList()[0]
print server.info()

# Try to connect...
s = BZFlag.Network.Socket()
s.connect(server.name)
hello = s.readStruct(BZFlag.Protocol.FromServer.HelloPacket)
print hello.__dict__
