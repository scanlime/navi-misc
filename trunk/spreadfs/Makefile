#
# This makefile compiles the python-fam extension module
#

PYTHON_INCLUDEDIR=`python -c 'import sys;print "/usr/include/python%d.%d" % sys.version_info[:2]'`

all: fam/_fam.so

fam/_fam.so: fam/_fam.c fam/_fam.h
	gcc -fPIC -shared -lfam -o fam/_fam.so fam/_fam.c -I $(PYTHON_INCLUDEDIR)

clean:
	rm -f fam/*.o fam/*.so

