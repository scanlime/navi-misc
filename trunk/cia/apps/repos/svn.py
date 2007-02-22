from cia.apps.repos.models import Repository
from django.conf import settings
from django.template import loader
from django.template.context import Context
import django.newforms as forms
import re, xmlrpclib, datetime
import pysvn, httplib, urlparse, urllib


class SvnClient:
    """This client talks to Subversion repositories, using settings
       and persistent state stored in a Repository model.
       """

    def __init__(self, model):
        self.model = model
        self.client = pysvn.Client()
        self.client.callback_ssl_server_trust_prompt = self._ssl_server_trust_prompt

    def _ssl_server_trust_prompt(self, trust_dict):
        retcode = True
        accepted_failures = ~0
        save = False
        return retcode, accepted_failures, save

    def probe(self, location=None):
        """Test the repository. On success, stores extra information
           about that repository including the root URL and the latest
           revision.
       
           This must be called any time the repository location is
           modified.  Since it resets our latest revision counter, all
           changes prior to this call will be ignored.

           On success, updates the repository model but does not save it.
           On failure, raises a form validation error.
           This function may block for an arbitrary amount of time!
           """
        location = location or self.model.location

        if not self.client.is_url(location):
            raise forms.ValidationError("Must be a valid URL")

        # XXX: svn+ssh might be useful too, but by default ssh will try to
        #      prompt interactively!

        scheme = location.split(':/', 1)[0]
        valid_schemes = ('http', 'https', 'svn')
        if not scheme in valid_schemes:
            raise forms.ValidationError("Only the %s, and '%s' URL schemes are allowed" % (
                ', '.join(["'%s'" % s for s in valid_schemes[:-1]]), valid_schemes[-1]))

        try:
            info = self.client.info2(location, recurse=False)[0][1]
        except pysvn.ClientError, e:
            raise forms.ValidationError(str(e))

        if info['kind'] != pysvn.node_kind.dir:
            raise forms.ValidationError("Must be a directory")

        self.model.root_url = info['repos_root_URL']
        self.model.last_revision = info['rev'].number
        self.model.uuid = info['repos_UUID']

    def poll(self):
        """Looks for updates to this repository since the last
           poll. Submits CIA messages for any commits that have
           occurred since then.
           """

        if not self.model.root_url:
            self.probe()
            self.model.save()

        # Have there been any new revisions? Get the last revision for
        # the whole repository. Note that this may be later than the
        # revision of the last commit we retrieve.

        last_revision = self._pollLatestRev()
        if last_revision <= self.model.last_revision:
            return

        changes = self.client.log(
            self.model.location,
            revision_start = pysvn.Revision(pysvn.opt_revision_kind.number, self.model.last_revision + 1),
            revision_end = pysvn.Revision(pysvn.opt_revision_kind.number, last_revision),
            discover_changed_paths = True,
            )

        for change in changes:
            self._deliverCommit(change)

        self.model.last_revision = last_revision
        self.model.last_update_time = datetime.datetime.now()
        self.model.save()

    _pollerData = ('<?xml version="1.0" encoding="utf-8"?>' +
                    '<propfind xmlns="DAV:">' +
                    '<prop><checked-in xmlns="DAV:"/></prop>' +
                    '</propfind>')

    _pollerUserAgent = 'CIA Repository Poller (http://cia.navi.cx)'

    _pollerRegex = re.compile('/!svn/bln/(\d+)</D:href>')

    def _pollLatestRev(self):
        """Determine the repository's latest revision. This version should
           always succeed if the repository is correct and the server is
           up. On failure, returns a pysvn error.
           """
        # Fast path...
        rev = self._pollLatestRevFast()
        if rev is not None:
            return rev
        
        info = self.client.info2(self.model.location, recurse=False)[0][1]
        return info['rev'].number

    def _pollLatestRevFast(self):
        """Do the minimal work possible in order to determine a repository's
           latest revision. This only works for HTTP repositories, and it may
           fail for any reason. If this fails, we have to fall back on pysvn.
           This function simply gives us a lighter-weight polling mechanism
           that works 90% of the time.

           Returns an integer revision, or None if we can't determine the
           revision with certainty. May also raise an exception if a network
           error occurs.
           """
        # Note the str(): sockets don't like Unicode hostnames.
        url = str(self.model.root_url + '/!svn/vcc/default')
        scheme, netloc, path, _, _, _ =  urlparse.urlparse(url)
        if scheme != 'http':
            return

        host, port = (netloc + ':80').split(':')[:2]

        http = httplib.HTTPConnection(host, int(port))
        http.putrequest('PROPFIND', path)
        http.putheader('User-Agent', self._pollerUserAgent)
        http.putheader('Content-Length', str(len(self._pollerData)))
        http.putheader('Depth', '0')
        http.endheaders()
        http.send(self._pollerData)

        response = http.getresponse()
        if response.status < 200 or response.status >= 300:
            response.close()
            return

        data = response.read()
        response.close()

        m = self._pollerRegex.search(data)
        if m:
            return int(m.group(1))

    def _deliverCommit(self, change):
        """Given pysvn's object describing a change log, generate and
           deliver a CIA XML message. This uses a Django template to
           do most of our conversion work.
           """

        path_info = {
            'module': self.model.default_module_name,
            'branch': None,
            }
        files = self._collectFiles(change['changed_paths'], path_info)

        root_url = self.model.root_url
        if root_url.split('://', 1)[0] not in ('http', 'https'):
            # Only use the repository root URL if it's something we expect
            # web browsers to understand...
            root_url = None

        revision_url = self.model.revision_url
        if revision_url:
            for key, value in (
                ("project", self.model.project_name),
                ("module", path_info['module']),
                ("branch", path_info['branch']),
                ("revision", change['revision'].number),
                ("author", change['author']),
                ):
                revision_url = revision_url.replace("{%s}" % key,
                                                    urllib.quote(str(value), safe=''))

        xml = loader.render_to_string('repos/svn.xml', Context({
            'timestamp': int(change['date']),
            'model': self.model,
            'change': change,
            'path_info': path_info,
            'files': files,
            'root_url': root_url,
            'revision_url': revision_url,
            }))

        xmlrpclib.ServerProxy(settings.CIA_RPC_URL).hub.deliver(xml)

    _pathRegexes = None

    def _updatePathRegexCache(self):
        """Update _pathRegexes. This is a list of compiled regular
           expressions taken from the lines of text in model.path_regexes.
           """
        if self._pathRegexes is not None:
            return
        self._pathRegexes = []

        if self.model.path_regexes:
            for line in self.model.path_regexes.split("\n"):
                self._pathRegexes.append(re.compile(line, re.VERBOSE))

    def _collectFiles(self, changed_paths, path_info):
        """Convert pysvn's chnaged_paths list into a list of
           File objects, and apply our path regexes on those files.
           """
        files = map(File, changed_paths)

        # Try each of our several regexes. To be applied, the same
        # regex must mach every file under consideration and they must
        # all return the same results. If we find one matching regex,
        # or we try all regexes without a match, we're done.

        self._updatePathRegexCache()
        for regex in self._pathRegexes:
            matches = matchAgainstFiles(regex, files)
            if matches is not None:
                path_info.update(matches)
                break

        return files


class File:
    """A changed file in a Subversion repository, constructed using
       changed_paths data from pysvn.
       """

    action_map = {
        'M': 'modify',
        'A': 'add',
        'D': 'remove',
        }

    def __init__(self, changed_path):
        full_path = changed_path['path']
        if full_path.startswith('/'):
            full_path = full_path[1:]

        self.full_path = full_path
        self.path = full_path
        self.action = self.action_map.get(changed_path['action'])


def matchAgainstFiles(regex, files):
    """Try matching a regex against all File objects in the provided list.
       If the regex returns the same matches for every file, the matches
       are returned in a dict and the matched portions are filtered out.
       If not, returns None.
       """
    prevMatchDict = None
    compiled = re.compile(regex, re.VERBOSE)
    for f in files:

        match = compiled.match(f.full_path)
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
        f.path = compiled.sub('', f.full_path)
    return prevMatchDict

