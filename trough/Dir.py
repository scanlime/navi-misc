#!/usr/bin/env python

import os, os.path, stat

suffices = ['mp3','ogg']

class File:
	def __init__(self, dir, parent=None):
		self.parent = parent

class Dir:
	def __init__(self, dir, parent=None):
		self.dir = dir
		self.parent = parent
		self.subdirs = []
		self.files = []
		if parent:
			self.path = os.path.join(parent.path,dir)
		else:
			self.path = dir

		self.read()
			
	def read(self):
		for child in os.listdir(self.path):
			if stat.S_ISDIR(os.stat(os.path.join(self.path,child)).st_mode):
				self.subdirs.append(Dir(child, self))
			else:
				for suffix in suffices:
					if child.find(suffix) != -1:
#						print os.path.join(self.path,child)
						self.files.append(child)
						break

	def getSubDirs(self):
		return self.subdirs

	def getFiles(self):
		return self.files

	def getFilePaths(self):
		result = self.files
		for i in xrange(len(result)):
			result[i]=self.path+'/'+result[i]
		return result

	def getPath(self):
		return self.path

	def getName(self):
		return self.dir

if __name__ == "__main__":
	import sys
	import time

	dir = []
	for x in sys.argv[1:]:
		dir.append(Dir(x))
