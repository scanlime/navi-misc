''' Trough.GroupTypes.None

    Each file gets its own glob
'''

__all__ = ['parse_shelf']

name = __name__.split('.').pop()

def parse_shelf (shelf, topdir, formats, default):
    result = []

    for dir in shelf[topdir].getDirectories():
	if (shelf[dir].getProperty('format') or default) == name:
	    result.extend(parse_shelf(shelf, dir, formats, default))
	else:
	    result.extend(formats[shelf[dir].getProperty('format') or default].parse_shelf(shelf, dir, formats, default))

    result.extend(parse_list(shelf[topdir].getFiles(shelf)))
    return result

def parse_list(filelist):
    if len(filelist) == 0:
	return ()

    result = []
    for file in filelist:
	result.append(tuple([file]))
    return result
