#!/usr/bin/env python
#
# CIA bot client script for Subversion repositories, written in python.
# This generates commit messages using CIA's XML commit format, and can
# deliver them using either XML-RPC or email.
#
# -- Micah Dowty <micah@navi.cx>
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
#      $REPOS/hooks/ciabot_svn.py "$REPOS" "$REV" &
#
#    Or, if you have multiple project hosted, you can add each
#    project's name to the commandline in that project's post-commit
#    hook:
#
#      $REPOS/hooks/ciabot_svn.py "$REPOS" "$REV" "My Project" &
#
############# There are some parameters for this script that you can customize:

class config:
    # Replace this with your project's name, or always provide a
    # project name on the commandline.
    project = "YOUR_PROJECT_NAME_HERE"

    # Subversion's normal directory hierarchy is powerful enough that
    # it doesn't have special methods of specifying modules, tags, or
    # branches like CVS does.  Most projects do use a naming
    # convention though that works similarly to CVS's modules, tags,
    # and branches.
    #
    # This is a list of regular expressions that are tested against
    # paths in the order specified. If a regex matches, the 'branch'
    # and 'module' groups are stored and the matching section of the
    # path is removed.
    #
    # Several common directory structure styles are below as defaults.
    # Uncomment the ones you're using, or add your own regexes.
    # Whitespace in the each regex are ignored.
    
    pathRegexes = [
    #   r"^ trunk/           (?P<module>[^/]+) ",
    #   r"^ (branches|tags)/ (?P<branch>[^/]+) ",
    #   r"^ (branches|tags)/ (?P<module>[^/]+)/ (?P<branch>[^/]+) ",
        ]
    
    # If your repository is accessable over the web, put its base URL here
    # and 'uri' attributes will be given to all <file> elements. This means
    # that in CIA's online message viewer, each file in the tree will link
    # directly to the file in your repository
    repositoryURI = None

    # This can be the http:// URI of the CIA server to deliver commits over
    # XML-RPC, or it can be an email address to deliver using SMTP. The
    # default here should work for most people. If you need to use e-mail
    # instead, you can replace this with "cia@cia.navi.cx"
    server = "http://cia.navi.cx"

    # The SMTP server to use, only used if the CIA server above is an
    # email address
    smtpServer = "localhost"

    # The 'from' address to use. If you're delivering commits via email, set
    # this to the address you would normally send email from on this host.
    fromAddress = "cia-user@localhost"

    # When nonzero, print the message to stdout instead of delivering it to CIA
    debug = 0


############# Normally the rest of this won't need modification

import sys, os, re, urllib


class File:
    """A file in a Subversion repository. According to our current
    configuration, this may have a module, branch, and URI in addition
    to a path."""

    # Map svn's status letters to our action names
    actionMap = {
        'U': 'modify',
        'A': 'add',
        'D': 'remove',
        }

    def __init__(self, fullPath, status=None):
        self.fullPath = fullPath
        self.path = fullPath
        self.action = self.actionMap.get(status)

    def getURI(self, repo):
        """Get the URI of this file, given the repository's URI. This
        encodes the full path and joins it to the given URI."""
        quotedPath = urllib.quote(self.fullPath)
        if quotedPath[0] == '/':
            quotedPath = quotedPath[1:]
        if repo[-1] != '/':
            repo = repo + '/'
        return repo + quotedPath

    def makeTag(self, config):
        """Return an XML tag for this file, using the given config"""
        attrs = {}

        if config.repositoryURI is not None:
            attrs['uri'] = self.getURI(config.repositoryURI)

        if self.action:
            attrs['action'] = self.action

        attrString = ''.join([' %s="%s"' % (key, escapeToXml(value,1))
                              for key, value in attrs.iteritems()])
        return "<file%s>%s</file>" % (attrString, escapeToXml(self.path))


class SvnClient:
    """A CIA client for Subversion repositories. Uses svnlook to
    gather information"""
    name = 'Python Subversion client for CIA'
    version = '1.15'

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
                              "From: %s\r\nTo: %s\r\n"
                              "Subject: DeliverXML\r\n\r\n%s" %
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
        return "<files>%s</files>" % ''.join([file.makeTag(self.config)
                                              for file in self.files])

    def svnlook(self, command):
        """Run the given svnlook command on our current repository and
        revision, returning all output"""
        return os.popen('svnlook %s -r "%s" "%s"' % \
                        (command, self.revision, self.repository)).read()

    def collectData(self):
        self.author = self.svnlook('author').strip()
        self.log = self.svnlook('log')
        self.diffLines = len(self.svnlook('diff').split('\n'))
        self.files = self.collectFiles()

    def collectFiles(self):
        # Extract all the files from the output of 'svnlook changed'
        files = []
        for line in self.svnlook('changed').split('\n'):
            path = line[2:].strip()
            if path:
                status = line[0]
                files.append(File(path, status))

        # Try each of our several regexes. To be applied, the same
        # regex must mach every file under consideration and they must
        # all return the same results. If we find one matching regex,
        # or we try all regexes without a match, we're done.
        matchDict = None
        for regex in self.config.pathRegexes:
            matchDict = matchAgainstFiles(regex, files)
            if matchDict is not None:
                self.__dict__.update(matchDict)
                break

        return files


def matchAgainstFiles(regex, files):
    """Try matching a regex against all File objects in the provided list.
       If the regex returns the same matches for every file, the matches
       are returned in a dict and the matched portions are filtered out.
       If not, returns None.
       """
    prevMatchDict = None
    compiled = re.compile(regex, re.VERBOSE)
    for f in files:

        match = compiled.match(f.fullPath)
        if not match:
            # Give up, it must match every file
            return None

        matchDict = match.groupdict()
        if prevMatchDict is not None and prevMatchDict != matchDict:
            # Give up, we got conflicting matches
            return None

        prevMatchDict = matchDict

    # If we got this far, the regex matched every file with
    # the same results.  Now filter the matched portion out of
    # each file and store the matches we found.
    for f in files:
        f.path = compiled.sub('', f.fullPath)
    return prevMatchDict


def escapeToXml(text, isAttrib=0):
    text = text.replace("&", "&amp;")
    text = text.replace("<", "&lt;")
    text = text.replace(">", "&gt;")
    if isAttrib == 1:
        text = text.replace("'", "&apos;")
        text = text.replace("\"", "&quot;")
    return text

if __name__ == "__main__":
    # Print a usage message when not enough parameters are provided.
    if len(sys.argv) < 3:
        sys.stderr.write("USAGE: %s REPOS-PATH REVISION [PROJECTNAME]\n" %
                         sys.argv[0])
        sys.exit(1)

    # If a project name was provided, override the default project name.
    if len(sys.argv) > 3:
        config.project = sys.argv[3]

    # Go do the real work.
    SvnClient(sys.argv[1], sys.argv[2], config).main()
