from django.db import models
from django.conf import settings
from django.contrib.auth.models import User
from django.contrib.contenttypes.models import ContentType
import urlparse, xmlrpclib


class UserAsset(models.Model):
    user = models.ForeignKey(User)

    content_type = models.ForeignKey(ContentType)
    object_id = models.PositiveIntegerField()    
    asset = models.GenericForeignKey()

    access = models.PositiveSmallIntegerField(choices=(
        (1, 'community'),
        (2, 'exclusive'),
        (3, 'trusted'),
        ), default=1)

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
        from cia.LibCIA.IRC import Network
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


class StatsTarget(models.Model):
    path = models.CharField(maxlength=255)
    # Metadata goes here...

    def __str__(self):
        return self.path

    class Admin:
        pass


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
    target = models.OneToOneField(StatsTarget)

    def __str__(self):
        return str(self.target)

    class Admin:
        pass

class Author(models.Model):
    objects = AssetManager()
    assets = models.GenericRelation(UserAsset)
    target = models.OneToOneField(StatsTarget)

    def __str__(self):
        return str(self.target)

    class Admin:
        pass


FILTER_UNKNOWN = 0     # Need to query server for ruleset
FILTER_INACTIVE = 1    # No ruleset  
FILTER_CUSTOM = 2
FILTER_PROJECT_LIST = 3

filter_mode_choices = (
    (FILTER_UNKNOWN,      'unknown'),
    (FILTER_INACTIVE,     'inactive'),
    (FILTER_CUSTOM,       'custom'),
    (FILTER_PROJECT_LIST, 'project list'),
    )

class Bot(models.Model):
    objects = AssetManager()
    assets = models.GenericRelation(UserAsset)

    network = models.ForeignKey(Network)
    location = models.CharField(maxlength=64, db_index=True)

    filter_mode = models.PositiveSmallIntegerField(
        choices=filter_mode_choices, default=FILTER_UNKNOWN)

    # For FILTER_CUSTOM. This is not a complete XML document,
    # just the contents of a <ruleset> element.
    custom_ruleset = models.TextField("Custom ruleset", blank=True)

    # For FILTER_PROJECT_LIST
    project_list = models.TextField("Project list", blank=True)
    show_project_names = models.BooleanField("Show project names", default=False)

    def getURI(self):
        s = self.network.uri
        if not s.endswith('/'):
            s += '/'

        # The CIA backend will accept 
        return s + self.location

    def syncFromServer(self):
        """Update this Bot from the RPC server, if necessary.
           Right now the only task this performs is to store the
           server's ruleset if filterMode is 'unknown'.
           """
        if self.filter_mode == FILTER_UNKNOWN:
            server = xmlrpclib.ServerProxy(settings.CIA_RPC_URL)
            ruleset = server.ruleset.getRuleset(self.getURI())

            if ruleset:
                # XXX: We should try to reduce the ruleset to one of
                #      the other FILTER_* modes if possible.
                self.filter_mode = FILTER_CUSTOM
                self.custom_ruleset = ruleset
                self.save()

            else:
                self.filter_mode = FILTER_INACTIVE
                self.save()

    def syncToServer(self):
        """Generate a ruleset according to this bot's filter
           settings, and upload that ruleset to the RPC server.
           """
        if self.filter_mode == FILTER_INACTIVE:
            ruleset = None

        else:
            ruleset = '<ruleset uri="%s">\n    <return/>\n</ruleset>' % self.getURI()
        
        server = xmlrpclib.ServerProxy(settings.CIA_RPC_URL)
        server.ruleset.store(settings.CIA_KEY, ruleset)

    def __str__(self):
        return "%s on %s" % (self.location, self.network)

    class Admin:
        list_display = ('network', 'location')
