#!/usr/bin/env python
#
# A simple benchmark for commit formatting
#

import os; os.chdir('..')
import sys; sys.path[0] += '/..'

import time
from LibCIA import Message, Formatters

messageText = """<?xml version='1.0' encoding='UTF-8'?>
<message><body><commit><author>johan</author><files>
<file action='modify'>pygtk/ChangeLog</file>
<file action='modify'>pygtk/NEWS</file>
</files>
<log>* codegen/codegen.py: Check arguments if we have no arguments.
Fixes bug #170266 [http://bugzilla.gnome.org/show_bug.cgi?id=170266] (Doug Quale)
* gtk/gdk.override:
* gtk/gtktreeview.override: gcc4 patches stolen from redhat
* NEWS: Updated
</log>
<url>http://cvs.gnome.org/bonsai/cvsquery.cgi?branch=&amp;dir=gnome-python&amp;who=johan&amp;date=explicit&amp;mindate=2005-03-15%2016:50&amp;maxdate=2005-03-15%2016:52</url>
</commit></body><generator><name>CIA Email Filters</name></generator>
<source><project>gnome</project><module>gnome-python</module></source>
<timestamp>1110923531</timestamp></message>
"""

r = range(200)
start = time.time()
for i in r:
    m = Message.Message(messageText)
    f = Formatters.getFactory().findMedium('xhtml', m).formatMessage(m)
end = time.time()


print "%.04f seconds/message" % ((end - start) / len(r))

### The End ###
