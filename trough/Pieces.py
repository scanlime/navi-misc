#!/usr/bin/env python

from Dir import Dir
from Piece import Piece
from types import *

class Pieces:
	''' Pieces
		Holds each Piece; has conversations with Dir to find them.
	'''
	def __init__ (self):
		self.pieces = []

	def addFilesSeparate (self, path, files):
		if type(files) is StringType:
			self.pieces.append(Piece(files,[files]))
		else:
			for file in files:
				self.pieces.append(Piece(file,file))
	
	def addFilesGrouped (self, path, files):
		if type(files) is StringType:
			self.pieces.append(Piece(path,[files]))
		else:
			self.pieces.append(Piece(path,files))

	def addDir (self, dir):
		''' Pieces.addDir
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
		subdirs = dir.getSubDirs()

		if files:
			if not subdirs:
				files.sort()
				if files[len(files)-1][0].isdigit():
					self.addFilesGrouped (dir.getName(),dir.getFilePaths())
				else:
					self.addFilesSeparate (dir.getName(),dir.getFilePaths())
			else:
				self.addFilesSeparate (dir.getPath(),dir.getFilePaths())
		
		for subdir in dir.getSubDirs():
			self.addDir (subdir)

	def addPaths (self, paths):
		for path in paths:
			self.addDir(Dir(path))

	def getCount (self):
		return len(self.pieces)

	def getPiece (self, num):
		return self.pieces[num]
		
if __name__ == "__main__":
	import sys
	import time

	p = Pieces()
	p.addPaths (sys.argv[1:])
