''' FileTree
    Objects for recursing through and storing a filesystem's heirarchial
    structure.
'''

__all__=['File','Directory']

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
    A single filesystem leaf, generally (but not necessarily) attached to a
    directory node.
	'''
    def __init__(self, name, parent=None, properties={}):
    	''' FileTree.File.__init__
            name (string)
		Name of this file
            parent (Directory):
		Optional parent Directory class (to be used by derived classes)
	'''
        self.parent = parent
        self.name    = name
	self.properties = properties
        self.callback = []

    def getName (self):
	''' FileTree.File.getName
            Returns the filename
	'''
        return self.name

    def getPath (self):
	''' FileTree.File.getPath
            Returns full path to the File
       	'''
	if self.parent:
            return os.path.join (self.parent.getPath(),self.name)
	return self.name

    def getParent (self):
	''' FileTree.File.getParent
	    Returns the File's parent node.
    	'''
        return self.parent

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
    def __init__(self, name, parent=None, properties={}):
    	''' FileTree.Directory.__init__

            name:	Deepest-level name of the entry in it's parent directory
            parent:	Optional parent Directory class
      	'''

        File.__init__(self, name, parent, properties)

        self.read()

    def read(self):
    	''' FileTree.Directory.read
		Read (and optionally recurse into) the contents of a directory.
		On return, files are sorted alphabetically, and directories
		are unsorted.
       	'''

	self.dirs = []
        self.files = []
	properties = ()

	# Temporarily changing the working directory is faster than creating
	# a full path from the filename, especially for big directories.
	oldwd = os.getcwd()
	os.chdir(File.getPath(self))

	entries = os.listdir('.')
	entries.sort()

	for entry in entries:
	    if os.path.isdir(entry):
		self.__new_dir__(entry)
	    else:
		if match_path(entry) == True:
		    self.__new_file__(entry)
		else:
		    if entry == PROPERTY_FILENAME:
			try:
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

    def getFiles(self):
    	''' FileTree.Directory.getFiles

	    Return the tuple containing the (sorted) file list
	'''
	result = []
	for x in self.files:
	    if not x.getProperty('mask'):
		result.append(x)
        return result

    def setProperty(self, key, value):
        ''' FileTree.Directory.setProperty

	    Recursively set a property on all files inside the directory
	'''
        File.setProperty(self, key, value)
	for x in self.dirs:
	    x.setProperty(key, value)
	for x in self.files:
	    x.setProperty(key, value)

    def find(self,path):
    	''' FileTree.Directory.find
		Returns a Directory structure for a file or directory with
		the given path, if found inside anywhere recursively within
		the current directory; returns None if the file was not found.

		TODO: This is still messy, and probably needs to be rewritten
       	'''
	path=os.path.abspath(path)
	p = path.split(self.getPath())
	if p[0] != '': return self
        return self.__find__(p[1][1:].split(os.path.sep))

    def inPath(self, path):
    	''' FileTree.Directory.inPath

            Returns True if path is somewhere inside the directory
            heirarchy.
       	'''
        return self.find(path) != None

    def destroy(self):
    	''' FileTree.Directory.destroy

            Annihilates the tree politely, making sure any files
            with callbacks associated with them are given fair
            warning.
	'''
        for i in xrange(0,len(self.dirs)):
            self.dirs[i].destroy()
        while len(self.files):
            l=len(self.files)
            self.files[0].destroy()
            if l == len(self.files):
                self.files.remove(self.files[0])

    def __find__(self,path):
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

    def __new_dir__(self, name):
    	''' FileTree.Directory.__new_dir__
		Allows new directories to be created in derived classes, with
		a derived instance (when overloaded by the derived class)
	'''
    	self.dirs.append(Directory(name, self, self.properties.copy()))

    def __new_file__(self, name):
    	''' FileTree.Directory.__new_file__
		Allows new directories to be created in derived classes, with
		a derived instance (when overloaded by the derived class)
	'''
    	self.files.append(File(name, self, self.properties.copy()))
