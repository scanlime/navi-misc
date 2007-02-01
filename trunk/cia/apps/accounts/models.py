from django.db import models
from django.conf import settings
from django.utils.html import escape
from django.contrib.auth.models import User
from django.contrib.contenttypes.models import ContentType
import django.newforms as forms
from cia.apps.stats.models import StatsTarget
import urlparse, xmlrpclib, re, difflib


class ACCESS:
    NONE = 0
    COMMUNITY = 1
    EXCLUSIVE = 2
    TRUSTED = 3

access_choices = (
    (ACCESS.NONE,       'Give up ownership'),
    (ACCESS.COMMUNITY,  'Community access'),
    (ACCESS.EXCLUSIVE,  'Exclusive access'),
    (ACCESS.TRUSTED,    'Trusted'),
    )

class UserAsset(models.Model):
    user = models.ForeignKey(User)

    content_type = models.ForeignKey(ContentType)
    object_id = models.PositiveIntegerField()    
    asset = models.GenericForeignKey()

    access = models.PositiveSmallIntegerField(choices = access_choices,
                                              default = ACCESS.COMMUNITY)

    date_added = models.DateTimeField(auto_now_add=True)
    trusted_by = models.DateTimeField(null=True, blank=True)

    def __str__(self):
        return "%s access to %s for %s" % (
            self.get_access_display(),
            self.asset,
            self.user)

    class Admin:
        pass


class NetworkManager(models.Manager):
    def importNetworks(self):
        """Import all network definitions from LibCIA into the database."""
        from LibCIA.IRC import Network
        for name, obj in Network.__dict__.iteritems():
            if (type(obj) is type(Network.BaseNetwork)
                and issubclass(obj, Network.BaseNetwork)
                and obj.alias):

                net = self.get_or_create(uri = "irc://%s/" % obj.alias)[0]
                net.description = name
                net.reviewed_by_admin = True
                net.created_by = None
                net.save()

class Network(models.Model):
    objects = NetworkManager()

    uri = models.CharField(maxlength=128)
    description = models.CharField(maxlength=200)

    is_popular = models.BooleanField(default=False)
    reviewed_by_admin = models.BooleanField(default=False)
    created_by = models.ForeignKey(User, null=True)
    date_added = models.DateTimeField(auto_now_add=True)

    def id_string(self):
        return str(self.id)
    
    def __str__(self):
        return self.description

    def getHost(self, requiredScheme):
        # We don't use urlparse here, since it doesn't
        # understand arbitrary URI schemes. For example:
        #
        #   >>> urlparse.urlsplit("irc://foo/bar")
        #   ('irc', '', '//foo/bar', '', '')
        #
        scheme, remainder = self.uri.split("://", 1)
        netloc = remainder.split("/", 1)[0]
        if scheme != requiredScheme:
            raise ValueError("The URI scheme %r is not supported. Expected %r." % (
                scheme, requiredScheme))
        return netloc

    class Admin:
        list_display = ('uri', 'description', 'reviewed_by_admin', 'created_by')


class AssetChangesetManager(models.Manager):
    def apply_changes(self, user, asset, changes={}, meta=()):
        """Apply a dictionary of changes to an asset. If anything in
           fact changed, this will create a record of those changes
           and save the asset.

           The optional 'meta' list contains a set of fields, with no
           data, to add to the changeset. This can be used to store
           ownership changes and other events which aren't directly
           represented by an asset's fields.
           """

        # Apply the changes while storing differences
        previous = {}
        for field, new in changes.items():
            p = getattr(asset, field, None)
            if p == new:
                del changes[field]
            else:
                previous[field] = p
                setattr(asset, field, new)

        self.store_changes(user, asset, changes, meta, previous)

        if changes:
            asset.save()

    def store_changes(self, user, asset, changes={}, meta=(), previous={}):
        """Like apply_changes, but don't actually modify
           the asset and don't prune anything from 'changes'.
           If no dictionary of previous values is provided,
           we assume all previous values were None. This is
           convenient for creating new assets.
           """           
        # Don't create empty changesets
        if not (changes or meta):
            return

        # Create the changeset
        cset = self.create(
            user = user,
            content_type = ContentType.objects.get_for_model(asset.__class__),
            object_id = asset.id,
            )

        # Create each change in the changeset
        for field, new in changes.items():
            AssetChangeItem.objects.create(
                changeset = cset,
                field = field,
                new_value = new,
                old_value = previous.get(field),
                )

        # Create meta-fields
        for field in meta:
            AssetChangeItem.objects.create(
                changeset = cset,
                field = field,
                )


class AssetChangeset(models.Model):
    """A single set of changes made to an asset, committed atomically
       to that asset by a single user. These log entries can be viewed
       by all, and they can be used to reconstruct old versions of an
       asset.
       """
    objects = AssetChangesetManager()

    time = models.DateTimeField(auto_now_add=True, db_index=True)
    user = models.ForeignKey(User)

    content_type = models.ForeignKey(ContentType)
    object_id = models.PositiveIntegerField(db_index=True)
    asset = models.GenericForeignKey()

    def __str__(self):
        return "Change %d for %s by %s" % (self.id, self.asset, self.user)


special_changes = {
    '_created':       ('/media/img/added-16.png',   "created"),
    '_gained_access': ('/media/img/added-16.png',   "gained access"),
    '_lost_access':   ('/media/img/removed-16.png', "lost access"),
    None:             ('/media/img/pencil-16.png',  "other change"),
    }

class AssetChangeItem(models.Model):
    """A separate AssetChange is used for every field touched by an
       AssetChangeset. Changesets which modify multiple fields at once
       will generate several AssetChangeItems.

       Fields here typically match the model's field names. Special field
       names, beginning with an underscore, represent events such as change
       in access level.
       """
    changeset = models.ForeignKey(AssetChangeset, related_name='items')
    field = models.CharField(maxlength=32, db_index=True)

    # Storing both the new value and old value is redundant, but it should
    # reduce the I/O load on the database relative to storing only diffs, since
    # we only have to visit a single row in order to get new value, old value,
    # or a diff. If this table gets too big we can revisit this problem later.
    #
    new_value = models.TextField(blank=True, null=True)
    old_value = models.TextField(blank=True, null=True)

    def is_special(self):
        return self.field[0] == '_'

    def get_icon(self):
        if self.is_special():
            try:
                return special_changes[self.field][0]
            except KeyError:
                pass
        return special_changes[None][0]

    def get_field(self):
        """Return the Field instance corresponding to self.field"""
        model = self.changeset.asset.__class__
        for f in model._meta.fields:
            if f.name == self.field:
                return f

    def get_description(self):
        if self.is_special():
            try:
                return special_changes[self.field][1]
            except KeyError:
                return special_changes[None][1]

        f = self.get_field()
        if f:
            return f.verbose_name
        else:
            return self.field

    def is_multiline(self):
        return isinstance(self.get_field(), models.TextField)

    def old_value_display(self):
        return self._display_value(self.old_value)

    def new_value_display(self):
        return self._display_value(self.new_value)

    def _display_value(self, value):
        """Format either the old or new value for display"""
        f = self.get_field()

        if f and f.choices:
            try:
                value = int(value)
            except ValueError:
                pass
            return dict(f.choices).get(value, value)

        return value

    def _append_diff_lines(self, chunk, prefix, style, source):
        for line in source:
            chunk.append({
                'prefix': prefix,
                'text': escape(line).rstrip().replace("  ", "&nbsp; "),
                'style': style,
                })

    def get_diff(self, context=3):
        """Compute a diff between old and new values, and return a sequence
           of dictionaries with 'text' and 'style' keys.
           """
        a = (self.old_value or '').rstrip().split("\n")
        b = (self.new_value or '').rstrip().split("\n")

        chunks = []
        for group in difflib.SequenceMatcher(None,a,b).get_grouped_opcodes(context):
            chunk = []
            chunks.append(chunk)

            for tag, i1, i2, j1, j2 in group:
                if tag == 'equal':
                    self._append_diff_lines(chunk, '&nbsp;&nbsp;', 'same', a[i1:i2])

                if tag == 'replace' or tag == 'delete':
                    self._append_diff_lines(chunk, '-&nbsp;', 'removed', a[i1:i2])

                if tag == 'replace' or tag == 'insert':
                    self._append_diff_lines(chunk, '+&nbsp;', 'added', b[j1:j2])

        return chunks

    def __str__(self):
        if self.new_value is None:
            return str(self.field)
        else:
            return "%s: %r -> %r" % (self.field, self.old_value, self.new_value)


class AssetManager(models.Manager):
    # A list of all Asset models, in the order they were created.
    models = []

    def contribute_to_class(self, model, name):
        models.Manager.contribute_to_class(self, model, name)
        self.models.append(model)
        model._meta.asset_type = model._meta.object_name.lower() + 's'

    def all_for_user(self, user):
        """Returns UserAsset objects for all assets of this type owned
           by a particular user.
           """
        ct = ContentType.objects.get_for_model(self.model)
        return UserAsset.objects.filter(user=user, content_type=ct)


class Project(models.Model):
    objects = AssetManager()
    assets = models.GenericRelation(UserAsset)
    target = models.ForeignKey(StatsTarget)

    def __str__(self):
        return str(self.target)

    class Admin:
        pass

class Author(models.Model):
    objects = AssetManager()
    assets = models.GenericRelation(UserAsset)
    target = models.ForeignKey(StatsTarget)

    def __str__(self):
        return str(self.target)

    class Admin:
        pass


class FILTER:
    UNKNOWN = 0     # Need to query server for ruleset
    INACTIVE = 1    # No ruleset
    CUSTOM = 2
    PROJECT_LIST = 3

filter_mode_choices = (
    (FILTER.UNKNOWN,      'Unknown'),
    (FILTER.INACTIVE,     'Inactive'),
    (FILTER.CUSTOM,       'Custom filter'),
    (FILTER.PROJECT_LIST, 'Filter by project'),
    )

def validate_ruleset(content, allow_empty=False):
    """Validate a custom ruleset, without sending it to the server."""
    from LibCIA import XML, Ruleset
    from xml.parsers.expat import ExpatError

    # Disable LibCIA's XPath cache- it will fill up quickly
    # if we cache every random XPath that we validate for our users.
    XML.enableXPathCache = False    

    # Wrap the ruleset using no newlines, so the line numbers match
    wrapped = "<ruleset>%s</ruleset>" % content

    # First, just try to compile the ruleset. This will catch a wide
    # variety of errors- we don't actively try yet to separate internal
    # errors from input errors. We would need to handle, at the least:
    #   - Well-formedness errors, from Expat
    #   - Validity errors, from LibCIA.XML
    #   - Formatter errors
    try:
        r = Ruleset.Ruleset(wrapped)
    except Exception, e:
        raise forms.ValidationError("%s: %s" % (e.__class__.__name__, e))

    # Empty rulesets will validate, but they're used as a special-case
    # to remove rulesets from the server's storage. We don't allow
    # them.
    if r.isEmpty() and not allow_empty:
        raise forms.ValidationError("Ruleset is empty")

    return content

def clean_up_text(text,
                  leading_lines_re = re.compile(r"^\s*\n"),
                  leading_spaces_re = re.compile(r"^\s*"),
                  ):
    """Clean up user-editable ruleset text. This expands tabs,
       removes blank leading lines, uniformly unindents as much
       as possible, and removes trailing whitespace from each line.
       """
    # Expand tabs and remove leading lines
    lines = leading_lines_re.sub("", text.expandtabs()).split("\n")

    # Measure the maximum unindent level
    level = None
    for line in lines:
        if line.strip():
            # Count leading spaces
            s = leading_spaces_re.match(line).span()[1]

            if level is None:
                level = s
            else:
                level = min(level, s)
    if level is None:
        return ""

    # Perform the unindentation on each line while stripping trailing space.
    return "\n".join([line[level:].rstrip() for line in lines])

class Bot(models.Model):
    objects = AssetManager()
    assets = models.GenericRelation(UserAsset)

    network = models.ForeignKey(Network)
    location = models.CharField(maxlength=64, db_index=True)

    filter_mode = models.PositiveSmallIntegerField(
        choices=filter_mode_choices, default=FILTER.UNKNOWN)

    # For FILTER.CUSTOM.
    # This is not a complete XML document, just
    # the contents of a <ruleset> element.
    custom_ruleset = models.TextField("Custom ruleset", blank=True)

    # For FILTER.PROJECT_LIST
    project_list = models.TextField("Project list", blank=True)
    show_project_names = models.BooleanField("Show project names", default=True, choices=(
        (False, 'No'),
        (True,  'Yes')))

    def getURI(self):
        s = self.network.uri
        if not s.endswith('/'):
            s += '/'
        return s + self.location

    def _matchProject(self, project):
        """Generate a ruleset filter for matching a single project"""
        return '<match path="/message/source/project">%s</match>' % escape(project)

    def _rulesetForProjectList(self):
        """Generates the contents of a ruleset representing a filter
           in the PROJECT_LIST mode.
           """
        lines = []

        # Match any of the projects, using an <or> if there's more than one.
        projects = self.project_list.split('\n')
        if len(projects) > 1:
            lines.append("<or>")
            for project in projects:
                lines.append('\t' + self._matchProject(project))
            lines.append("</or>")
        else:
            lines.append(self._matchProject(projects[0]))

        lines.append('<formatter medium="irc"/>')
        if self.show_project_names:
            lines.append('<formatter name="IRCProjectName"/>')

        return '\n'.join(lines)

    def syncFromServer(self):
        """Update this Bot from the RPC server, if necessary.
           Right now the only task this performs is to store the
           server's ruleset if filterMode is 'unknown'.
           """
        if self.filter_mode != FILTER.UNKNOWN:
            return

        ruleset = self._loadRuleset()
        if not ruleset:
            # If there's no ruleset, mark the bot as inactive.
            self.filter_mode = FILTER.INACTIVE
            self.save()
            return

        # Parse the ruleset using LibCIA's XML library
        from LibCIA import XML
        dom = XML.parseString(ruleset)

        # XXX: We should try to reduce the ruleset to one of
        #      the other FILTER.* modes if possible. For now,
        #      we'll always import existing rulesets as
        #      FILTER.CUSTOM.

        # Flatten the contents of the <ruleset> element, clean up
        # the resulting text, and save that as a custom filter.

        text = ''.join([n.toxml() for n in dom.documentElement.childNodes])
        self.filter_mode = FILTER.CUSTOM
        self.custom_ruleset = clean_up_text(text)
        self.save()

    def syncToServer(self):
        """Generate a ruleset according to this bot's filter
           settings, and upload that ruleset to the RPC server.
           """
        if self.filter_mode == FILTER.INACTIVE:
            self._storeRuleset(None)
            
        elif self.filter_mode == FILTER.CUSTOM:
            self._storeRuleset(self.custom_ruleset)

        elif self.filter_mode == FILTER.PROJECT_LIST:
            self._storeRuleset(self._rulesetForProjectList())

    def _wrapRuleset(self, content):
        """Wrap a ruleset with its outer <ruleset> element"""
        return '<ruleset uri="%s">\n%s\n</ruleset>' % (escape(self.getURI()), content)

    def _loadRuleset(self):
        """Retrieve this bot's ruleset from the server. It will be
           returned as a complete XML document with <ruleset/> element
           and optional processing instructions.
           """
        server = xmlrpclib.ServerProxy(settings.CIA_RPC_URL)
        return server.ruleset.getRuleset(self.getURI())

    def _storeRuleset(self, content):
        """Send a ruleset to the server, if necessary.  If 'content'
           evaluates to False, any existing ruleset will be deleted.
           """
        server = xmlrpclib.ServerProxy(settings.CIA_RPC_URL)
        uri = self.getURI()

        if not content:
            # The bot server will fail to unset a ruleset
            # that doesn't exist. Avoid that situation here...
            if not server.ruleset.getRuleset(uri):
                return
            content = ''

        server.ruleset.store(settings.CIA_KEY,
                             self._wrapRuleset(content))

    def __str__(self):
        return "%s on %s" % (self.location, self.network)

    class Admin:
        list_display = ('network', 'location')
