import FSTree
from Piece import Piece

from types import *
import sys
import os.path

class Pieces:
	''' Pieces
		Holds each Piece; has conversations with FSTree.Directory to find them.
		Does not inherit pickles.
	'''
	def __init__ (self):
		self.roots = []
		self.pieces = []

	def addFilesSeparate (self, name, files):
		if type(files) is not list:
			self.pieces.append(Piece(files,[files],self))
		else:
			for file in files:
				name = os.path.splitext(file.getName())[0]
				self.pieces.append(Piece(name,file,self))
	
	def addFilesGrouped (self, name, files):
		if type(files) is not list:
			self.pieces.append(Piece(name,[files],self))
		else:
			self.pieces.append(Piece(name,files,self))

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
			for i in xrange(0,len(self.roots)):
				if self.roots[i].inPath(path) == 1:
					self.roots[i].destroy()
					self.roots.remove(self.roots[i])
			tree = FSTree.Directory(path)
			self.addDirectory(tree)
			self.roots.append(tree)
#		print 'found ',len(self.pieces)

	def getCount (self):
		return len(self.pieces)

	def getPiece (self, num):
		return self.pieces[num]

	def removePaths (self, paths):
		for path in paths:
			for i in xrange(0,len(self.roots)):
				dir = self.roots[i].find(path)
				if dir:
					dir.destroy();
					break
			else:
				sys.stderr.write('warning: '+path+' not found in tree\n')

	def breakPieces (self, paths):
		for path in paths:
			for p in self.pieces:
				if(os.path.commonprefix([p.getWhere()[0].getPath(),path]) == path): 
					self.addFilesSeparate(p.getWhere()[0].getPath(),p.getWhere())
					self.delPiece(p)
	

	def delPiece (self, piece):
		'''	Pieces.delPiece

			Called by individual pieces to make sure they're cleaned
			up happily.
		'''
		self.pieces.remove(piece)
