#!/usr/bin/env python

import distutils.core
from Trough import version

data_files=[('bin',['trough']),
	    ('share/trough',['share/trough/properties']),
	    ('doc/trough',['doc/trough/properties.example'])]

distutils.core.setup(name="Trough",
		     version=version,
		     packages=['Trough','Trough.Formats'],
		     data_files=data_files,
		     description="A directory tree random selection tool",
		     license="LGPL",
		     author="Daniel Kuester",
		     author_email="Daniel.Kuester@colorado.edu",
		     contact="Daniel Kuester",
		     contact_email="Daniel.Kuester@colorado.edu")
