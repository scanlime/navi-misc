#!/usr/bin/env python

import types

class Group:
    ''' Group
 	A single group of files, i.e.:
		* One classical, multi-movement work
		* One jazz or pop album
		..etc..
	Group is most useful when used by GroupList, for which it is a
	container class.
    '''

    def __init__ (self, name, where, callback):
    	''' Group.__init__
	    Initialize Group with all of the data it's likely to need.
	    
	    name (string)
		Name of the group
	    where (string or list of strings)
		Path to file(s) in the group
		TODO: this should really be a tuple, not a list
	    callback (GroupList.GroupList)
	    	This is some crackful, crackful stuff that I haven't even
		refigured out what it does
	    '''
	self.name = name
	self.callback = callback
		
	if type(where) is list:
		self.where = where
	else:
		self.where = [where]

	# Register up some callbacks
	for i in xrange(0,len(self.where)):
	    self.where[i]._register_del_callback(self)
		
    def getWhere (self):
    	''' Group.getWhere
	    Returns the very same where argument Group was passed on
	    initialization.
	    '''
    	return self.where

    def getName (self):
        ''' Group.getName
	    Returns the name of our Group.  Yay.
	    '''
    	return self.name

    def removed (self,where):
    	''' Group.removed
	    This is where the crack happens.  Inform our parents that we've
	    decided to jump ship.
	    '''
        self.where.remove(where)
        if len(self.where) == 0:
	    self.callback.delGroup(self)
