#!/usr/bin/env python

import types

class Piece:
	''' Piece
		Grouped files
	'''

	def __init__ (self, name, where, callback):
		self.name = name
		self.callback = callback
		
		if type(where) is list:
			self.where = where
		else:
			self.where = [where]

		# Register up some callbacks
		for i in xrange(0,len(self.where)):
			self.where[i]._register_del_callback(self)
		
	def getWhere (self):
		return self.where

	def getName (self):
		return self.name

	def removed (self,where):
		self.where.remove(where)
		if len(self.where) == 0:
			self.callback.delPiece(self)
