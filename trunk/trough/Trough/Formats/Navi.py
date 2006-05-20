''' Trough.GroupTypes.Navi
	Digs through navi's filesystem sorting scheme.

	As it turns out, with navi's somewhat arbitrarily restrictive naming
	conventions, it's really easy to dig out groups of files.
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

    result.extend(parse_list(shelf, shelf[topdir].getFiles(shelf)))
    return result

def parse_list(shelf, filelist):
    if len(filelist) == 0:
	return ()

    name = shelf[filelist[len(filelist)-1]].getName()
    if name[0] >= '0' and name[0] <= '9':
	return [tuple(filelist)]
    else:					# files ain't numbered
	result = []
	for file in filelist:
	    result.append(tuple([file]))
        return result
