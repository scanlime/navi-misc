''' Trough.GroupTypes.Navi
	Digs through navi's filesystem sorting scheme.

	As it turns out, with navi's somewhat arbitrarily restrictive naming
	conventions, it's really easy to dig out groups of files.
'''

import __init__
__all__ = ['parse_tree']

def parse_tree (tree, name):
    result = []
    for dir in tree.getDirectories():
	if dir.getProperty('format') == name:
	    result.extend(parse_tree(dir, name))
	else:
	    result.extend(__init__.parse_tree(dir))

    result.extend(parse_list(tree.getFiles()))
    return result

def parse_list(filelist):
    return [tuple(filelist)]
