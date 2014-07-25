#!/usr/bin/env python
#
# XWT (XML Widget Template) compiler frontend
#
# Currently very dumb, specify the input and output files
# on the command line, nothing more, nothing less.

import PicoGUI, sys
from PicoGUI import XWTParser

parser = XWTParser.XWTParser()
wt = parser.Parse(open(sys.argv[1]).read())
open(sys.argv[2],'w').write(wt);
