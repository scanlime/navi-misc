#!/bin/sh
wc -l src/*.cpp src/*.h modules/*.py media_src/*.ths media_src/*.xwt util/*.py util/testworlds/*.py | sort -n
