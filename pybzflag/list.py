#!/usr/bin/env python
import BZFlag.ListServer 
for server in BZFlag.ListServer.getDefault().list():
    print server.info()

