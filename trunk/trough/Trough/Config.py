import FileTree
import cPickle as pickle
import os, os.path
import sys
import shutil

class Config:
    def __init__(self,confdir=os.path.join(os.environ['HOME'],'.trough')):
	self.confdir=confdir
	self.trees=[]
	self.properties=[]
	pass

    def readTrees(self):
    	try:
	    file = self.__open_config__('trees','r')
	except:
	    try:
		self.__make_config__('trees')
	    except:
 	        print '** warning: couldn\'t open saved directory tree(s) **'
	    return
	try:
		newtrees = pickle.load(file)
		self.trees = newtrees
	except:
		print '** warning: saved directory tree(s) empty or unreadable **'
	file.close()

    def writeTrees(self):
    	file = self.__open_config__('trees','w')
	pickle.dump(self.trees,file,True)
	file.close()

    def getTrees(self):
	return self.trees

    def readProperties(self):
    	try:
	    file = self.__open_config__('properties','r')
	except:
	    try:
		self.__make_config__('properties')
	    except:
 	        print '** warning: couldn\'t open saved tree properties **'
	    return

	try:
	    self.properties = eval(file.read())
	except:
	    print '** error: invalid properties config file'
	    sys.exit(1)

	file.close()

    def addTree(self, path):
    	t = self.findTreeContaining(path)
    	if t:
	    if len(path) < len(t.getPath()): # the new path includes our path
		self.trees.remove(t)
	    else:
		print '** warning: path \''+path+'\' is already in a tree, skipping'
		return
	self.trees.append(FileTree.Directory(path))
	self.writeTrees()

    def removeTree(self, path):
    	t = self.findTreeRoot(path)
	if t == None:
	    print "** warning: no tree found rooted at ``"+path+"'' **"
	    return
	self.trees.remove(t)
	self.writeTrees()

    def findTreeContaining(self, path):
	for x in self.trees:
	    if x.inPath(path):
		return x
	return None

    def findTreeRoot(self, path):
    	for x in self.trees:
	    if x.getPath() == path:
		return x
    	return None

    def applyProperties(self):
    	for prop in self.properties:
	    tree = self.findTreeContaining(prop[0])
	    if tree == None:
		print "** warning: no tree found containing `"+prop[0]+"'"
		continue
	    tree.find(prop[0]).setProperty(prop[1][0],prop[1][1])

    def __open_config__(self, name, mode):
	if not os.path.isdir(self.confdir):
	    os.mkdir(self.confdir)
	return open(os.path.join(self.confdir,name), mode)

    def __make_config__(self, name):
    	shutil.copyfile(name,os.path.join(self.confdir,name))
