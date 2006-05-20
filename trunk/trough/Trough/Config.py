import FileTree
import cPickle as pickle
import os, os.path
import sys
import shutil
import shelve

class Config:
    def __init__(self,confdir=os.path.join(os.environ['HOME'],'.trough')):
	self.confdir=confdir
	self.properties=[]
	self.shelf = shelve.open(self.__get_config__('trees'))
	pass

    def readProperties(self):
    	try:
	    file = open(self.__get_config__('properties'), 'r')
	except:
	    try:
		self.__make_default_config__('properties')
	    except:
 	        print '** warning: couldn\'t create preferences file **'
	    return

	try:
	    self.properties = eval(file.read())
	except:
	    print '** error: invalid properties config file'
	    sys.exit(1)

	file.close()

    def addTree(self, path):
	# Is the parent in the shelf already?
	parentPath = os.path.split(path)[0]
	if not self.shelf.has_key(parentPath):
	    # Nope, it's not!
	    parentPath = None

	# Recurse & Shelve
	self.shelf[path] = FileTree.Directory(path, self.shelf, parentPath)

    def getShelf():
	return self.shelf

    def applyProperties(self):
    	for prop in self.properties:
	    path = os.path.normpath(prop[0])
	    print path,self.shelf[path]
	    if not self.shelf.has_key(path):
		print '** warning: no shelved object "%s"' % path
		continue
	    try:
		self.shelf[path].setProperty(self.shelf, prop[1][0],prop[1][1])
	    except str:
		print '** warning: unable to set property "%s" for "%s"' % prop

    def __get_config__(self, name):
	if not os.path.isdir(self.confdir):
	    os.mkdir(self.confdir)
	return os.path.join(self.confdir,name)

    def __make_default_config__(self, name):
    	n = os.path.join('share', 'trough', name)
	try:
	    shutil.copyfile(n,os.path.join(self.confdir,name))
	except:
	    shutil.copyfile(os.path.join(sys.exec_prefix,n),os.path.join(self.confdir,name))
