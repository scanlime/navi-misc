#!/bin/sh
mencoder snapshot.nuv -o snapshot.avi -ovc lavc -lavcopts vcodec=mpeg4:vbitrate=2000
