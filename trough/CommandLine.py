#!/usr/bin/env python

import optik
import sys
from PlayList import PlayList

version = '0.0.0'

class CommandLine:
	def __init__(self):
		self.parser = optik.OptionParser (version=version)

		self.parser.add_option('-a','--add',dest='add',action="store_true",
							   help="recursively add path(s) to the playlist")
#		self.parser.add_option('-p','--playlist',dest="path",
#							   help="path to playlist file")
		self.parser.add_option('-r','--remove',dest='remove',action="store_true",
							   help="recursively remove path(s) from the playlist")
#		self.parser.add_option('-v','--verbose',dest="verbose",
#							   help="give verbose output")
		(opts,self.args) = self.parser.parse_args()

		self.playlist = PlayList()

		print self.playlist.getPieces()

		if opts.add:
			self.add()
			
		if opts.remove:
			self.remove()


	def add(self):
		print 'adding',self.args
		self.playlist.getPieces().addPaths(self.args)
		self.playlist.dump()
		sys.exit(0)

	def remove(self):
#		playlist.getPieces().removePaths(self.args)
#		playlist.dump()
		sys.exit(0)
