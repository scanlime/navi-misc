from cia.apps.repos.models import Repository
from django.conf import settings
from django.template import loader
import re, pysvn, xmlrpclib


class SvnClient:
    """This client talks to Subversion repositories, using settings
       and persistent state stored in a Repository model.
       """

    def __init__(self, model):
        self.client = pysvn.Client()
        self.model = model

    def probe(self):
        """Test the repository. On success, stores extra information about
           that repository including the root URL and the latest revision.
       
           This must be called any time the repository location is
           modified.  Since it resets our latest revision counter, all
           changes prior to this call will be ignored.

           On success, saves the repository. On failure, raises a pysvn error.
           """
        client = pysvn.Client()


        repo_info = pysvn.Client().info2(repos.locationrepository, recurse=False)[0][1]
        config.repositoryURI = repo_info['repos_root_URL']
        latest_rev = repo_info['rev'].number    
    




loader.render_to_string(template_name, context).lstrip().split("\n", 1)











########################

class File:
    """A changed file in a Subversion repository, constructed using
       changed_paths data from pysvn.
       """

    # Map svn's status letters to our action names
    actionMap = {
        'U': 'modify',
        'A': 'add',
        'D': 'remove',
        }

    def __init__(self, changed_path):
        fullPath = changed_path['path']
        if fullPath.startswith('/'):
            fullPath = fullPath[1:]

        self.fullPath = fullPath
        self.path = fullPath
        self.action = self.actionMap.get(changed_path['action'])

    def getURI(self, repo):
        """Get the URI of this file, given the repository's URI. This
           encodes the full path and joins it to the given URI.
           """
        quotedPath = urllib.quote(self.fullPath)
        if quotedPath[0] == '/':
            quotedPath = quotedPath[1:]
        if repo[-1] != '/':
            repo = repo + '/'
        return repo + quotedPath

    def makeTag(self, repo=None):
        """Return an XML tag for this file. The optional repository URI
           will be used if it was specified.
           """
        attrs = {}

        if config.repositoryURI is not None:
            attrs['uri'] = self.getURI(config.repositoryURI)

        if self.action:
            attrs['action'] = self.action

        attrString = ''.join([' %s="%s"' % (key, escapeToXml(value,1))
                              for key, value in attrs.items()])
        return "<file%s>%s</file>" % (attrString, escapeToXml(self.path))


def store_repos_info(projectModel):
    """Given a Project model, poll that project's subversion repository
       for updates. This will update the model's state information
       as necessary, and send commit messages for any new projects
       we haven't seen before.

       Assumes that the project is configured for CLIENT_TYPE.SERVER_SVN.
       """
    

    

class RemoteSvnClient:
    """A CIA client for remote Subversion repositories, using pysvn."""

    def __init__(self, project):
        client = pysvn.Client()


        repo_info = client.info2(repository, recurse=False)[0][1]
        config.repositoryURI = repo_info['repos_root_URL']
        latest_rev = repo_info['rev'].number

        self.commits = client.log(repository,
                                  revision_start = pysvn.Revision(pysvn.opt_revision_kind.number, revision),
                                  revision_end = pysvn.Revision(pysvn.opt_revision_kind.number, latest_rev),
                                  discover_changed_paths = True)


class CommitTranslator:
    """Translates Subversion commits from pysvn's format to CIA's XML messages."""

    name = 'Server-side Subversion interface for CIA'
    version = '0.1'

    def __init__(self, projectModel, commit):
        self.project = project
        self.commit = commit

    def getMessage(self):
        return ("<message>" +
                self.makeGeneratorTag() +
                self.makeSourceTag() +
                self.makeBodyTag() +
                "</message>")

    def deliver(self):
        xmlrpclib.ServerProxy(settings.CIA_RPC_URL).hub.deliver(self.getMessage())

    def makeAttrTags(self, *names):
        """Given zero or more attribute names, generate XML elements for
           those attributes only if they exist and are non-None.
           """
        s = ''
        for name in names:
            if hasattr(self, name):
                v = getattr(self, name)
                if v is not None:
                    s += "<%s>%s</%s>" % (name, escapeToXml(v), name)
        return s

    def makeGeneratorTag(self):
        return "<generator>%s</generator>" % self.makeAttrTags(
            'name',
            'version',
            )

    def makeSourceTag(self):
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
            'url',
            ),
            self.makeFileTags(),
            )

    def makeFileTags(self):
        """Return XML tags for our file list"""
        return "<files>%s</files>" % ''.join([file.makeTag(self.config)
                                              for file in self.files])

    def collectData(self, commit):
        self.log = commit['message']
        self.files = self.collectFiles(commit['changed_paths'])
        self.author = commit['author']
        self.revision = commit['revision'].number
        self.project = self.config.project
        if self.config.revisionURI is not None:
            self.url = self.config.revisionURI % self.__dict__
        else:
            self.url = None

    def collectFiles(self, changed_paths):
        files = map(File, changed_paths)

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
    text = unicode(text)
    text = text.replace("&", "&amp;")
    text = text.replace("<", "&lt;")
    text = text.replace(">", "&gt;")
    if isAttrib == 1:
        text = text.replace("'", "&apos;")
        text = text.replace("\"", "&quot;")
    return text

if __name__ == "__main__":
    RemoteSvnClient("http://svn.navi.cx/misc/trunk/", 11240, config).main()

### The End ###
