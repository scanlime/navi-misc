#!/usr/bin/env python

import Formats
import random

class GroupList:
	''' GroupList.GroupList
		bla
	'''
	def __init__(self):
	    self.pieces = []

	def add(self, tree):
	    self.pieces.extend(Formats.parse_tree(tree))
#	    for x in self.pieces:
#		for y in x:
#		    print y.getPath()

	def random(self):
	    s = random.randint(0,len(self.pieces)-1)
	    return self.pieces[s]

	def getCount(self):
	    return len(self.pieces)
