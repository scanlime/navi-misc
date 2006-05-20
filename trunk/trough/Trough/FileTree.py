''' FileTree
    Objects for recursing through and storing a filesystem's heirarchial
    structure.
'''

__all__ = ['File', 'Directory']

import os, os.path

PROPERTY_FILENAME = '.trough'
SUFFICES = ['mp3','ogg']    # TODO:	There should really be a better way
			    #		of matching paths in the future..
def match_path(path):
	for suffix in SUFFICES:
	    if path[len(path)-len(suffix):] == suffix:
		return True
	return False

class File:
    ''' FileTree.File
	A single filesystem leaf, possibly attached to a directory node.
	'''
    def __init__(self, name, parentPath=None, properties={}):
    	''' FileTree.File.__init__
            name (string)
		Name of this file
            parent (Directory):
		Function reference to get stuff
	'''
	self.parentPath = parentPath or os.path.split(name)[0]
        self.name = parentPath and name or os.path.split(name)[1]
	self.properties = properties
        self.callback = []

    def getName (self):
	''' FileTree.File.getName
            Returns the file name (go figure)
	'''
        return self.name

    def getPath (self):
	''' FileTree.File.getPath
	    Returns full path to the File
       	'''
	if self.parentPath:
            return os.path.join (self.parentPath,self.getName())
	return self.name

    def getParentPath (self):
	''' FileTree.File.getParentPath
	    Returns the File's parent node.
    	'''
        return self.parentPath

    def getProperty (self, key):
	if not self.properties.has_key(key):
	    return None
        return self.properties[key]

    def setProperty (self, key, value):
        self.properties[key] = value

    def __eq__ (self,target):
    	return self.name == target

class Directory (File):
    ''' FileTree.Directory
    '''
    def __init__(self, name, shelf, parentPath=None, properties={}):
    	''' FileTree.Directory.__init__

            name:	Unqualified directory name
            parentPath:	Optional callable that returns parent directory path

      	'''

        File.__init__(self, name, parentPath, properties)

        self.read(shelf)

    def read(self, shelf):
    	''' FileTree.Directory.read
		Recursively store the contents of a directory.
		On return, files are sorted alphabetically, and directories
		are unsorted.
       	'''

	self.dirs = []
        self.files = []
	properties = ()
	print self.getPath()

	if shelf.has_key(self.getPath()):
	    shelf[self.getPath()].remove(shelf)

	# Temporarily changing the working directory is faster than creating
	# a full path from the filename, especially for big directories.
	oldwd = os.getcwd()
	os.chdir(File.getPath(self))

	entries = os.listdir('.')
	entries.sort()

	for entry in entries:
	    if os.path.isdir(entry):
		self.__new_dir__ (entry, shelf)
	    else:
		if match_path(entry) == True:
		    self.__new_file__(entry, shelf)
		else:
		    if entry == PROPERTY_FILENAME:
			try: # did i really write this? ugh
			    f = file(entry)
			    properties = eval(f.read())
			    f.close()
			except:
			    properties = ()
			    print '** warning: unable to read',		\
				  os.path.join(os.getcwd(),entry),	\
				  'to read directory properties'
			    continue
			for p in properties:
			    try:
				File.setProperty(self,p[0],p[1])
			    except:
				print '** warning: reading tree: invalid key/value pair:', \
				property, '(skipping) **'

	os.chdir(oldwd)

	# Now that all the little'uns are loaded, we can apply some properties.
	# Hurray!

    def getDirectories(self):
        return self.dirs

    def getFiles(self, shelf):
    	''' FileTree.Directory.getFiles

	    Return the tuple containing the (sorted) file list
	'''
	result = []
	for x in self.files:
	    if not shelf[x].getProperty('mask'):
		result.append(x)
        return result

    def setProperty(self, shelf, key, value):
        ''' FileTree.Directory.setProperty

	    Recursively set a property on all files inside the directory
	'''
	File.setProperty(self, key, value)
	for x in self.dirs:
	    shelf[x].setProperty(shelf, key, value)
	for x in self.files:
	    shelf[x].setProperty(key, value)

    def removeChildren (self, shelf):
	for entry in self.dirs:
	    shelf[entry].removeChildren(shelf)
	    del shelf[entry]
	for entry in self.files:
	    del shelf[entry]

    def remove(self, shelf):
    	''' FileTree.Directory.destroy

            Annihilates the tree politely, making sure any files
            with callbacks associated with them are given fair
            warning.
	'''
	self.removeChildren(shelf)
	del shelf[self.getPath()]

    def __find__(self, path):
	''' FileTree.Directory.__find__
		Search recursively for a file.

		path: Array of file or directory names, arranged heirarchially
		      from the root downwards.
	'''
	if len(path) == 0:
	    return self

	i = None
	try:
		i = self.dirs.index(path[0])
	except:
		pass
	if i != None: return self.dirs[i].__find__(path[1:])

	i = None
	try:
		i = self.files.index(path[0])
	except:
		pass
	if i != None: return self.files[i]
	return None

    def __new_dir__ (self, name, shelf):
	''' FileTree.Directory.__new_dir__
		Allows new directories to be created in derived classes, with
		a derived instance (when overloaded by the derived class)
	'''
    	entry = Directory(name, shelf, self.getPath(), self.properties.copy())
	self.dirs.append(entry.getPath())
	shelf[entry.getPath()] = entry

    def __new_file__ (self, name, shelf):
    	''' FileTree.Directory.__new_file__
		Allows new directories to be created in derived classes, with
		a derived instance (when overloaded by the derived class)
	'''
    	entry = File(name, self.getPath(), self.properties.copy())
	self.files.append(entry.getPath())
	shelf[entry.getPath()] = entry
