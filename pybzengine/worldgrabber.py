#!/usr/bin/env python
#
# A simple utility to save a server's world file
#
from BZFlag import CommandLine, Client
import sys

argParser = CommandLine.Parser(Client.PlayerClient,
                               callSign = "@Worldgrabber")

argParser.add_option("-o", "--output", dest="outputFile", metavar="FILE", default="-",
                     help="Sets the name of the file to save the downloaded world to.")

client = argParser.parse()
fileName = client.cmdLineValues['outputFile']

if fileName == '-':
    f = sys.stdout
else:
    f = open(fileName, "w")

def grabWorld():
    f.write("# Grabbed by PyBZFlag Worldgrabber from %s\n\n" % client.options['server'])
    client.game.world.saveText(f)
    client.eventLoop.stop()

client.game.world.onLoad.observe(grabWorld)
client.run()
