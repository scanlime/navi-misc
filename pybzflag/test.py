#!/usr/bin/env python

import BZFlag.Network
import BZFlag.Client
import BZFlag.ListServer

# Just pick the first server on the list that's compatible with us
server = BZFlag.ListServer.getDefault().filteredList()[0]
print server.info()

# Connect a client
client = BZFlag.Client.BaseClient(server.name)
print client.id
client.disconnect()

