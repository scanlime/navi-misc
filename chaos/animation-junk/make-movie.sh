#!/bin/sh
ls *.png | sort > list
transcode -i list -x imlist,null -g 320x240 -y xvid,null -f 30 -o movie.avi -H 0

