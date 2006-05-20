# CRACKFUL
#
# Thank you, that, is all.

import string
import sys

__all__=['parse_tree']

formats = ['All','Navi','None']
default = 'Navi'
libname=__name__

# this might be automajick someday
def list_formats():
    return formats

def parse_shelf(topdir, shelf):
    if not shelf.has_key(topdir):
	print '** warning: shelf has no key "%s" for parse_tree' % topdir
	return []
    fmt = shelf[topdir].getProperty('format')
    formats = {}

    for format in list_formats():
	try:
	    formats[format] = load_module(format)
	except:
	    print '** warning: unable to load format module "'+mod+'" **'

    return formats[shelf[topdir].getProperty('format') or default].parse_shelf(shelf, topdir, formats, default)

def load_module(name):
    modname = libname+'.'+name
    mod = __import__(modname)
    components = modname.split('.')
    for comp in components[1:]:
	mod = getattr(mod, comp)
    return mod
