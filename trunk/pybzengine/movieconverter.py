#!/usr/bin/env python
#
# Simple utility to transcode (recompress) movies recorded with the
# in-game movie recorder. This requires mencoder to be installed
# correctly with libavcodec.
#
from BZEngine.UI import MovieRecorder
from BZEngine import optik
import sys, re

parser = optik.OptionParser(usage = "usage: %prog [options] snapshot.bzm")
parser.add_option("-o", "--output", metavar="FILE", dest="file",
                  help="Sets the output AVI file")
parser.add_option("-r", "--resolution", metavar="WIDTHxHEIGHT", dest="resolution",
                  help="Sets the output resolution. Default is 512x384.", default="512x384")
parser.add_option("-b", "--bitrate", metavar="KBPS", dest="bitrate",
                  help="Sets the mpeg4 encoder bit rate. Default is 900kbps.", default=900)
(options, args) = parser.parse_args()

if len(args) == 0:
    parser.error("One or more recording filenames must be specified on the command line.")

for inFile in args:
    if options.file:
        outFile = options.file
    else:
        outFile = re.sub("\.[^\/\\\.]+$", "", inFile) + ".avi"

    (width, height) = options.resolution.split("x")

    t = MovieRecorder.Transcoder(inFile)
    t.encode("-o %s -vop scale=%s:%s -ovc lavc -lavcopts vcodec=mpeg4:vbitrate=%s" %
             (outFile, width, height, options.bitrate))
