''' Trough.GroupTypes.None

    Each file gets its own group
'''

__all__ = ['parse_tree']

name = __name__.split('.').pop()

def parse_tree (tree, formats, default):
    result = []

    for dir in tree.getDirectories():
	if (dir.getProperty('format') or default) == name:
	    result.extend(parse_tree(dir, formats, default))
	else:
	    result.extend(formats[dir.getProperty('format') or default].parse_tree(dir, formats, default))

    result.extend(parse_list(tree.getFiles()))
    return result

def parse_list(filelist):
    if len(filelist) == 0:
	return ()

    result = []
    for file in filelist:
	result.append(tuple([file]))
    return result
