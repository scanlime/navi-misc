#!/usr/bin/env python

import FSTree
from Piece import Piece
from types import *

class Pieces:
	''' Pieces
		Holds each Piece; has conversations with FSTree.Directory to find them.
		Does not inherit pickles.
	'''
	def __init__ (self):
		self.pieces = []

	def addFilesSeparate (self, name, files):
		if type(files) is not list:
			self.pieces.append(Piece(files,[files]))
		else:
			for file in files:
				self.pieces.append(Piece(name,file))
	
	def addFilesGrouped (self, name, files):
		if type(files) is not list:
			self.pieces.append(Piece(name,[files]))
		else:
			self.pieces.append(Piece(name,files))

	def addDirectory (self, dir):
		''' Pieces.addDirectory
			Add the contents of a directory, sorting them into groups as necessary.
			
			The rules for file grouping are fairly simple; the files become a group 
			(i.e., multi-movement classical piece or a jazz/pop album) if:
			a) Their directory contains no subdirectories
			b) They are all numbered.

			For efficiency's sake, we just check whether the last file in the sorted
			file list is numbered; if so, assume (perhaps unfairly) that everything in
			the directory should be a group.
		'''
		files = dir.getFiles()
		subdirs = dir.getDirectories()

		if files:
			if not subdirs:
				files.sort()
				if files[len(files)-1].getName()[0].isdigit():
#					print dir.getFiles().getPath()
					self.addFilesGrouped (dir.getName(),dir.getFiles())
				else:
					self.addFilesSeparate (dir.getName(),dir.getFiles())
			else:
				self.addFilesSeparate (dir.getPath(),dir.getFiles())
		
		for subdir in dir.getDirectories():
			self.addDirectory (subdir)

	def addPaths (self, paths):
		for path in paths:
			self.addDirectory(FSTree.Directory(path))
#		print 'found ',len(self.pieces)

	def delPieces (self, pattern):
		pass

	def getCount (self):
		return len(self.pieces)

	def getPiece (self, num):
		return self.pieces[num]
		
if __name__ == "__main__":
	import sys
	import time

	p = Pieces()
	p.addPaths (sys.argv[1:])
