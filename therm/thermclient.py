#!/usr/bin/env python
import xmlrpclib, time

s = xmlrpclib.ServerProxy("http://navi.picogui.org:4510")

for therm, reading in s.getAverages().items():
    print "%30s : %.01f F" % (therm, reading)
