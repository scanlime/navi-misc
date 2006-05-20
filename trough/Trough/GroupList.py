#!/usr/bin/env python

import Formats
import random

class GroupList:
	''' GroupList.GroupList
		bla
	'''
	def __init__(self):
	    self.pieces = []

	def load(self, path, shelf):
	    self.pieces.extend(Formats.parse_shelf(path, shelf))
#	    for x in self.pieces:
#		for y in x:
#		    print y.getPath()

	def random(self):
	    s = random.randint(0,len(self.pieces)-1)
	    return self.pieces[s]

	def getCount(self):
	    return len(self.pieces)
