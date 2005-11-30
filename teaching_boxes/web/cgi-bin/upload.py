#!/usr/bin/env python
import cgi
import cgitb
cgitb.enable()
import os, sys

form = cgi.FieldStorage()
if not form.has_key("upfile"):
    sys.exit(0)

fileitem = form['upfile']

fout = file (os.path.join ('/home/darkstar62/tmp', fileitem.filename), "wb")
while 1:
    chunk = fileitem.file.read(100000)
    if not chunk:
        break
    fout.write (chunk)
fout.close()

print "Content-type: text/plain\n"


