#!/usr/bin/env python

import sys
import commands
import xmlrpclib
import string

################

debug = False
server = "http://cia.navi.cx"

# you may wish to use your computers name here, as arch archives are often personal in nature
project = "littlegreen"

###############    

archname = sys.argv[1]

category = commands.getoutput("tla parse-package-name -c "+archname)
branch = commands.getoutput("tla parse-package-name -b "+archname)
revision = commands.getoutput("tla parse-package-name -l "+archname)
if revision[0:len("patch-")] == "patch-": revision = revision[len("patch-"):]

myid = commands.getoutput("tla my-id -u")

def send(msg):
    if debug:
        print msg
        return
    xmlrpclib.ServerProxy(server).hub.deliver(msg)

def escapeToXml(text, isAttrib=0):
    text = text.replace("&", "&amp;")
    text = text.replace("<", "&lt;")
    text = text.replace(">", "&gt;")
    if isAttrib == 1:
        text = text.replace("'", "&apos;")
        text = text.replace("\"", "&quot;")
    return text

# grab the log message from tla
patchlog = commands.getoutput("tla cat-archive-log "+archname+" | formail -X Summary: | sed 's/Summary: //'")

msg = """
<message>
  <generator> <name>arch</name> <version>1.2</version> </generator>
  <source> <project>%s</project> <module>%s</module> <branch>%s</branch> </source>
  <body>
    <commit>
      <revision>%s</revision>
      <author>%s</author>
      <log>%s</log>
    </commit>
  </body>
</message>
""" % (project, category, branch, revision, escapeToXml(myid), escapeToXml(patchlog))

send(msg)
