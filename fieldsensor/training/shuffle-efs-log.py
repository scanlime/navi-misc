#!/usr/bin/env python
#
# Randomly split one tracker log into two pieces
#   ./shuffle-efs-log.py <input> <ratio of len(output1) to len(input)> <output 1> <output 2>
#
# --Micah
#

import sys, random

input = open(sys.argv[1], "r")
ratio = float(sys.argv[2])
out1 = open(sys.argv[3], "w")
out2 = open(sys.argv[4], "w")

lines = input.readlines();
random.shuffle(lines)
part1_length = int(len(lines) * ratio + 0.5)
part1 = lines[:part1_length]
part2 = lines[part1_length:]

out1.write("".join(part1))
out2.write("".join(part2))
