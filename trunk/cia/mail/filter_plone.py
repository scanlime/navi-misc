#!/usr/bin/env python
from filter_python import PythonFilter

# As far as I can tell, plone and python are using the same commit format
class PloneFilter(PythonFilter):
    project = 'plone'

if __name__ == '__main__':
    PloneFilter().main()
