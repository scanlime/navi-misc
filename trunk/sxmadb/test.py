#!/usr/bin/env python
from Ft.Xml.Domlette import NonvalidatingReader
from Ft.Xml.XPath.Context import Context
from Ft.Xml.XPath import Evaluate

doc = NonvalidatingReader.parseUri('file:///home/jupiter/projects/sxmadb/techniques/clutching-feathers.xml')
ctx = Context(doc)
stances = Evaluate('//punch|//stance|//strike', context=ctx)
for stance in stances:
    print stance.childNodes[0].nodeValue
