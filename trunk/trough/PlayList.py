#!/usr/bin/env python
''' Playlist
	Handles Playlist happiness.  Helpful, isn't this?
'''

import os
import cPickle
import Pieces

DefaultPath = os.path.join(os.environ['HOME'],'.trough-playlist')

class PlayList:
	def __init__ (self, path=DefaultPath):
		self.path=path
		
		try:
			self.playlist = cPickle.load(open(path,'r'))
		except:
			self.playlist = Pieces.Pieces()

	def dump (self):
		cPickle.dump(self.playlist,open(self.path,'w'),1)

	def getPieces (self):
		return self.playlist
