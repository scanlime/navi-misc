''' FSTree
	Objects for recursing through and storing a filesystem's heirarchial
	structure.
'''

import os, os.path

suffices = ['mp3','ogg']	# TODO: Make this less silly

class File:
	''' FSTree.File
		A single filesystem leaf, generally (but not necessarily) attached to a 
		directory node.
	'''
	def __init__(self, name, parent=None):
		''' FSTree.File.__init__
			
			name:	Deepest-level name of the entry in it's parent directory, 
			parent:	Optional parent Directory class (to be used by derived
					classes)
		'''
		self.parent = parent
		self.name	= name

	def getName (self):
		''' FSTree.File.getName

			Returns the filename.
		'''
		return self.name

	def getPath (self):
		''' FSTree.File.getPath
			
			Returns the full path to the File.
		'''
		if self.parent:
			return os.path.join (self.parent.getPath(),self.name)
		return self.name

	def getParent (self):
		''' FSTree.File.getParent
			
			Returns the File's parent node.
		'''
		return self.parent

class Directory (File):
	''' FSTree.Directory
	'''
	def __init__(self, name, parent=None):
		''' FSTree.Directory.__init__

			name:	Deepest-level name of the entry in it's parent directory, 
			parent:	Optional parent Directory class
		'''
		
		File.__init__(self, name, parent)
		
		self.subdirs = []
		self.files = []

		self.recurse()

	def recurse(self):
		''' FSTree.Directory.recurse

			Recurses through all of the files and subdirectories in this
			Directory, storing the heirarchial structure along the way.
		'''

		path = File.getPath(self)
		try:
			entries = os.listdir(path)
		except:
			print 'Unable to read ',path
			return

		entries.sort()

		for i in xrange(0,len(entries)):
#
#			print entries[i]
			if os.path.isdir(os.path.join(path,entries[i])):
				self.subdirs.append(Directory(entries[i], self))
			else:
				for suffix in suffices:
					if entries[i].find(suffix) != -1:
						self.files.append(File(entries[i],self))
						break

	def getDirectories(self):
		return self.subdirs

	def getFiles(self):
		return self.files

if __name__ == "__main__":
	import sys
	import time

	dir = []
	for x in sys.argv[1:]:
		dir.append(Directory(x))
