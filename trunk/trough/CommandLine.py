''' CommandLine
    Transparently load either the python 2.2 or 2.3 command line parser.
    '''

try:	# python 2.3 case
	from optparse import *
except:	# revert to python 2.2 case
	from optik import *
