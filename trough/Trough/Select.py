#!/usr/bin/env python

import random
import sys
from types import *
from Pieces import Pieces

def Select(pieces):
	if pieces.getCount() == 0:
		sys.stderr.write("warning: no files to choose from\n")
		return []
	s = random.randint(0,pieces.getCount()-1)
	return pieces.getPiece(s).getWhere()
	 

if __name__ == "__main__":
	import sys
	
	p = Pieces()
	p.addPaths (sys.argv[1:])

	s = Select(p)

	s = Select(p)
	if type(s) is StringType:
		s = [s]
	for x in s:
		print x

