#!/usr/bin/python
import fieldbox

# Simple test
sensor = fieldbox.sensor()
while True:
    packet = sensor.read()
    for reading in packet:
        n = int(reading * 900)
        print "%0.8f %s" % (reading, "#" * n)
    print "\n-------------------------------\n"

