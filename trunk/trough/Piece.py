#!/usr/bin/env python

import types

class Piece:
	''' Piece
		Conglomeration of media.
	'''

	def __init__ (self, name, where):
		self.name = name
		
		if type(where) is list:
			self.where = where
		else:
			self.where = [where]
		
	def getWhere (self):
		return self.where

	def getName (self):
		return self.name
