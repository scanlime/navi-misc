#!/usr/bin/env python
''' Playlist
	Handles Playlist happiness.  Helpful, isn't this?
'''

import os
import cPickle as pickle
import Pieces

DefaultPath = os.path.join(os.environ['HOME'],'.trough-playlist')

class PlayList:
	def __init__ (self, path=DefaultPath):
		if path is None:
		    self.path=DefaultPath
		else:
		    self.path=path
		
		try:
			self.playlist = pickle.load(open(self.path,'r'))
		except:
			self.playlist = Pieces.Pieces()

	def dump (self):
		print 'self.path=',self.path
		pickle.dump(self.playlist,open(self.path,'w'),1)

	def getPieces (self):
		return self.playlist
