#!/usr/bin/env python

import random
from types import *
from Pieces import Pieces

def Select(pieces):
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

