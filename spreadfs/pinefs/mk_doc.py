#!/usr/bin/env python

import imp

files = ['pyfs', 'rpcgen']

for nm in files:
    mod_fil, pathnm, descr = imp.find_module (nm)
    mod = imp.load_module (nm, mod_fil, nm, descr)
    html_fil = file (nm + '.html', "w")
    docstr = mod.__doc__.replace ('<', '&lt;').replace ('>', '&gt;')
    html_fil.write ("""<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
<head>
<title>
Pinefs - %s
</title>
</head>
<body>
<H3>%s</H3>
<p>
<pre>
%s
</pre>
<p>
<a href="README.html">Back to Pinefs home page</a>
</body>
</html>""" % (nm, nm, docstr))

    
