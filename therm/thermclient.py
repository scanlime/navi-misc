#!/usr/bin/env python
import xmlrpclib, time

s = xmlrpclib.ServerProxy("http://navi.picogui.org:4510")

# Get information about the current averaging period
times = s.getTimes()
print "%0.2f seconds until next averaging period\n" % (
    times['averagePeriod'] + times['periodStart'] - times['time'])

# Get dictionaries mapping sensor IDs to their descriptions and their averages.
# Sensors with no data available don't show up in the averages dict.
desc = s.getDescriptions()
avg = s.getAverages()
for id in desc.keys():
    if avg.has_key(id):
    	reading = "%.01f F" % avg[id]
    else:
        reading = "No data"
    print "%30s : %s" % (desc[id], reading)
