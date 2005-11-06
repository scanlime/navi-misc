#!/usr/bin/env python
import _fidtool
import os

fd = os.open("foo", os.O_CREAT | os.O_RDWR, 0666)
#dataset = map(int, open("cia-commits.dataset"))
dataset = [2] * 1024

_fidtool.append_samples(fd, dataset)

os.close(fd)
