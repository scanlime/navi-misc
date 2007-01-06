from django.db import models
from django.contrib.auth.models import User
from django.contrib.contenttypes.models import ContentType

class UserAsset(models.Model):
    user = models.ForeignKey(User)

    content_type = models.ForeignKey(ContentType)
    object_id = models.PositiveIntegerField()    
    asset = models.GenericForeignKey()

    access = models.PositiveSmallIntegerField(choices=(
        (1, 'Community'),
        (2, 'Exclusive'),
        (3, 'Trusted'),
        ))
    date_added = models.DateTimeField(auto_now_add=True)
    trusted_by = models.DateTimeField(null=True)

    def __str__(self):
        return "%s access to %s for %s" % (
            self.get_access_display(),
            self.asset,
            self.user)

    class Admin:
        pass

class Network(models.Model):
    uri = models.CharField(maxlength=200)
    description = models.CharField(maxlength=200, null=True)

    def __str__(self):
        return self.description or self.uri

    class Admin:
        list_display = ('uri', 'description')

class StatsTarget(models.Model):
    path = models.CharField(maxlength=255)
    # Metadata goes here...

    def __str__(self):
        return self.path

    class Admin:
        pass

class Project(models.Model):
    assets = models.GenericRelation(UserAsset, object_id_field='target_id')
    target = models.OneToOneField(StatsTarget)

    def __str__(self):
        return str(self.target)

    class Admin:
        pass

class Author(models.Model):
    assets = models.GenericRelation(UserAsset, object_id_field='target_id')
    target = models.OneToOneField(StatsTarget)

    def __str__(self):
        return str(self.target)

    class Admin:
        pass

class Bot(models.Model):
    assets = models.GenericRelation(UserAsset)

    network = models.ForeignKey(Network)
    location = models.CharField(maxlength=50)

    useCustomRuleset = models.BooleanField("Use custom ruleset")
    customRuleset = models.TextField("Custom ruleset", blank=True)
    showProjectNames = models.BooleanField("Show project names")
    projects = models.ManyToManyField(Project, blank=True)
    authors = models.ManyToManyField(Author, blank=True)

    def getURI(self):
        return "%s/%s" % (self.network.uri, self.location)

    def __str__(self):
        return "%s on %s" % (self.name, self.network)

    class Admin:
        list_display = ('network', 'location')
