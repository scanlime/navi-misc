#!/usr/bin/env python
#
# CIA bot client script for Subversion repositories, written in python.
# This generates commit messages using CIA's XML commit format, and can
# deliver themusing either XML-RPC or email.
#
# -- Micah Dowty <micah@picogui.org>
#
# This script is cleaner, more featureful, and faster than the shell
# script version, but won't work on systems without Python or that don't
# allow outgoing HTTP connections.
#
# To use the CIA bot in your Subversion repository...
#
# 1. Customize the parameters below
#
# 2. This script should be called from your repository's post-commit
#    hook with the repository and revision as arguments. For example,
#    you could copy this script into your repository's "hooks" directory
#    and add something like the following to the "post-commit" script,
#    also in the repository's "hooks" directory:
#
#      REPOS="$1"
#      REV="$2"
#      $REPOS/hooks/ciabot_svn.pl "$REPOS" "$REV"&
#
############# There are some parameters for this script that you can customize:

class config:
    # Always replace this with your project's name
    project = "YOUR_PROJECT_NAME_HERE"

    # Subversion's normal directory hierarchy is powerful enough that it doesn't
    # have special methods of specifying modules, tags, or branches like CVS does.
    # Most projects do use a naming convention though that works similarly to CVS's
    # modules, tags, and branches.
    #
    # If non-None, this is a regex that matches a directory, with group names
    # 'branch' and 'module'. If it matches, it is replaced with the empty string
    # and the groups, if present, are stored in the commit message.
    #
    # For example, in a repository containing paths like:
    #   trunk/module1
    #   trunk/module2
    #   branches/moose
    #   tags/plastic-42
    #
    # The following regex would extract the module and branch, leaving
    # the filename within the particular branch/module:
    #
    #   pathRegex = r"""
    #               ^(                                        # Always start at the beginning of the path
    #                 ( trunk/ (?P<module>[^/]+) ) |          # This might be a module in the trunk...
    #                 ( (branches|tags)/ (?P<branch>[^/]+) )  # ...or a branch or a tag
    #               )/?                                       # Slurp up the slash if one is present, so it doesn't appear in the file path
    #               """
    #
    # Note that whitespace and comments in the regex are ignored.
    #
    pathRegex = None

    # This can be the http:// URI of the CIA server to deliver commits over
    # XML-RPC, or it can be an email address to deliver using SMTP. The
    # default here should work for most people. If you need to use e-mail
    # instead, you can replace this with "cia@cia.navi.cx"
    server = "http://cia.navi.cx"

    # The SMTP server to use, only used if the CIA server above is an email address
    smtpServer = "localhost"

    # The 'from' address to use. If you're delivering commits via email, set
    # this to the address you would normally send email from on this host.
    fromAddress = "cia-user@localhost"

    # When nonzero, print the message to stdout instead of delivering it to CIA
    debug = 0


############# Normally the rest of this won't need modification

import sys, os, re


class SvnClient:
    """A CIA client for Subversion repositories. Uses svnlook to gather information"""
    name = 'Python Subversion client for CIA'
    version = '1.1'

    def __init__(self, repository, revision, config):
        self.repository = repository
        self.revision = revision
        self.config = config

    def deliver(self, message):
        if config.debug:
            print message
        else:
            server = self.config.server
            if server.startswith('http:') or server.startswith('https:'):
                # Deliver over XML-RPC
                import xmlrpclib
                xmlrpclib.ServerProxy(server).hub.deliver(message)
            else:
                # Deliver over email
                import smtplib
                smtp = smtplib.SMTP(self.config.smtpServer)
                smtp.sendmail(self.config.fromAddress, server,
                              "From: %s\r\nTo: %s\r\n\r\n%s" %
                              (self.config.fromAddress, server, message))

    def main(self):
        self.collectData()
        self.deliver("<message>" +
                     self.makeGeneratorTag() +
                     self.makeSourceTag() +
                     self.makeBodyTag() +
                     "</message>")

    def makeAttrTags(self, *names):
        """Given zero or more attribute names, generate XML elements for
           those attributes only if they exist and are non-None.
           """
        s = ''
        for name in names:
            if hasattr(self, name):
                v = getattr(self, name)
                if v is not None:
                    s += "<%s>%s</%s>" % (name, escapeToXml(str(v)), name)
        return s

    def makeGeneratorTag(self):
        return "<generator>%s</generator>" % self.makeAttrTags(
            'name',
            'version',
            )

    def makeSourceTag(self):
        self.project = self.config.project
        return "<source>%s</source>" % self.makeAttrTags(
            'project',
            'module',
            'branch',
            )

    def makeBodyTag(self):
        return "<body><commit>%s%s</commit></body>" % (
            self.makeAttrTags(
            'revision',
            'author',
            'log',
            'diffLines',
            ),
            self.makeFileTags(),
            )

    def makeFileTags(self):
        """Return XML tags for our file list"""
        return "<files>%s</files>" % ''.join(["<file>%s</file>" % file for file in self.files])

    def svnlook(self, command):
        """Run the given svnlook command on our current repository and revision, returning all output"""
        return os.popen('svnlook %s -r "%s" "%s"' % (command, self.revision, self.repository)).read()

    def collectData(self):
        self.author = self.svnlook('author').strip()
        self.log = self.svnlook('log')
        self.diffLines = len(self.svnlook('diff').split('\n'))
        self.files = self.collectFiles()

    def collectFiles(self):
        # Extract all the files from the output of 'svnlook changed'
        files = []
        for line in self.svnlook('changed').split('\n'):
            line = line[2:].strip()
            if line:
                files.append(line)

        if self.config.pathRegex:
            # We have a regex we'll be applying to each path to try to extract information out of it.
            # We only apply any of the modifications if the regex matches each path with the same results.
            prevMatchDict = None
            regex = re.compile(self.config.pathRegex, re.VERBOSE)
            for file in files:
                match = regex.match(file)
                if not match:
                    # Give up, it must match every file
                    return files
                matchDict = match.groupdict()
                if prevMatchDict is not None and prevMatchDict != matchDict:
                    # Also give up, it must match each time with the same results
                    return files
                prevMatchDict = matchDict

            # If we got this far, the regex matched every file with the same results.
            # Now filter the matched portion out of each file and store the matches we found.
            self.__dict__.update(prevMatchDict)
            return [regex.sub('', file) for file in files]
        else:
            return files


def escapeToXml(text, isAttrib=0):
    text = text.replace("&", "&amp;")
    text = text.replace("<", "&lt;")
    text = text.replace(">", "&gt;")
    if isAttrib == 1:
        text = text.replace("'", "&apos;")
        text = text.replace("\"", "&quot;")
    return text

if __name__ == "__main__":
    SvnClient(sys.argv[1], sys.argv[2], config).main()
