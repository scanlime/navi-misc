#!/usr/bin/env python


class Piece:
	''' Piece
		A single (n-movement) classical piece or jazz/pop album.
	'''

	def __init__ (self,name,files):
		self.name = name
		self.paths = files
		
	def getPaths (self):
		return self.paths

	def getName (self):
		return self.name
