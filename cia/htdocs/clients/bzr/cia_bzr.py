#!/usr/bin/env python
# Simple post-commit hook for bzr that submits the commit to 
# CIA (http://cia.vc/)

# Requires a recent version of bzr (0.7 or higher)

# Copyright (C) 2005 Jelmer Vernooij <jelmer@samba.org>
# Published under the GNU GPL

# Installation:
# Copy this file to ~/.bazaar/plugins

# Configuration:
# In ~/.bazaar/branches.conf, set 'cia_project' to a sane value for the branch 
# you would like to use CIA on and enable the post_commit message by setting 
# post_commit to something like 'bzrlib.plugins.cia_bzr.post_commit'

# If 'cia_project' is not set for a branch, no message will be sent.

# Other options that can be set are 'cia_server' and 'cia_user' 

# Example branches.conf:
#[/home/charis/jelmer/bitlbee/jelmer]
#post_commit = bzrlib.plugins.cia_bzr.post_commit
#cia_project = BitlBee

import sys
import xmlrpclib
import bzrlib
from xml.sax import saxutils
from warnings import warn

def post_commit(branch, revision_id):
    config = bzrlib.config.BranchConfig(branch)
    revision = branch.get_revision(revision_id)

    project = config.get_user_option('cia_project')
    if not project:
        return

    server = config.get_user_option('cia_server')
    if not server:
        server = "http://cia.vc"

    author = config.get_user_option('cia_user')
    if not author:
        author = config.username()

    revno = branch.revision_id_to_revno(revision_id)
    files = []
    delta = branch.get_revision_delta(revno)

    [files.append(f) for (f,_,_) in delta.added]
    [files.append(f) for (f,_,_) in delta.removed]
    [files.append(f) for (_,f,_,_,_,_) in delta.renamed]
    [files.append(f) for (f,_,_,_,_) in delta.modified]

    msg = """
<message>
  <generator> 
    <name>bzr</name> 
    <version>%s</version> 
    <url>http://samba.org/~jelmer/bzr/cia_bzr.py</url>
  </generator>
  <source>
    <project>%s</project>
    <module>%s</module>
  </source>
  <body>
    <commit>
      <revision>%s</revision>
      <files>%s</files>
      <author>%s</author>
      <log>%s</log>
    </commit>
  </body>
</message>
""" % (bzrlib.version_string, project, branch.nick, revision_id, 
        "\n".join(["<file>%s</file>" % saxutils.escape(f) for f in files]), 
        saxutils.escape(author), 
        saxutils.escape(revision.message))

    xmlrpclib.ServerProxy(server).hub.deliver(msg)
