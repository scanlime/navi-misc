#!/usr/bin/python

libs = ['SDL', 'db_cxx-3.2', 'GL', 'GLU']
cppflags = '-g'

env = Environment(CPPFLAGS = cppflags, LIBS = libs)
Export('env')
SConscript('src/SConscript')
