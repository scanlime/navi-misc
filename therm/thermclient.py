#!/usr/bin/env python
import xmlrpclib, time

s = xmlrpclib.ServerProxy("http://navi.picogui.org:4510")

try:
    s.setAveragePeriod(1)
except:
    pass
time.sleep(2)
print s.getAverages()
