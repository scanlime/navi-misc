#!/bin/sh
LD_PRELOAD=./glx-hijack.so PY_MODULE=overlay PYTHONPATH=`pwd` mplayer -vo gl $*
