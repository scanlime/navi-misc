#!/usr/bin/env python
from xml.dom.minidom import parse, getDOMImplementation
import os, sys

if len(sys.argv) != 2:
    print 'Usage:',sys.argv[0],'directory'
    sys.exit(1)

impl = getDOMImplementation()

doc = impl.createDocument(None, sys.argv[1], None)
top = doc.documentElement

files = os.listdir(sys.argv[1])
for file in files:
    if file.endswith('.xml'):
        dom = parse(sys.argv[1] + '/' + file)
        technique = dom.getElementsByTagName(sys.argv[1][:-1])[0].cloneNode(False)
        child = top.appendChild(technique)
        child.setAttribute('href', file)
        dom.unlink()

f = open(sys.argv[1] + '.xml', 'w')
f.write('<?xml version="1.0"?>\n\n')
f.write(top.toprettyxml())
f.close()
