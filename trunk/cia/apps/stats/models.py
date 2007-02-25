from django.db import models
from cia.apps.images.models import ImageSource
from django.newforms.util import smart_unicode, StrAndUnicode


class StatsTarget(StrAndUnicode, models.Model):
    """Refers to stats stored by the stats subsystem. This is a
       location in the database where information about a project,
       author, etc. are stored.

       Right now these stats paths are intimately tied to stats
       metadata and to the actual URL used by a stats page.
       Eventually this table will become a reference to a stored
       Esquilax query along with an individual URL.
       """
    path = models.CharField(maxlength=255, db_index=True)

    # User-editable Stats metadata
    title = models.CharField(maxlength=128, null=True, blank=True)
    subtitle = models.CharField(maxlength=128, null=True, blank=True)
    url = models.CharField(maxlength=255, null=True, blank=True)
    description = models.TextField(null=True, blank=True)
    photo = models.ForeignKey(ImageSource, null=True, related_name='targets_by_photo')
    icon = models.ForeignKey(ImageSource, null=True, related_name='targets_by_icon')

    # Internal Stats Metadata
    links_filter = models.TextField(null=True, blank=True)
    related_filter = models.TextField(null=True, blank=True)

    def get_default_title(self):
        return self.path.rsplit('/', 1)[-1]

    def get_asset_set(self):
        """Return a QuerySet for the assets associated with this stats target,
           or None of this path has no corresponding asset type.
           """
        if self.path.startswith('project/'):
            return self.project_set
        if self.path.startswith('author/'):
            return self.author_set

    def is_editable(self):
        """A stats target is considered 'editable' if there are
           no exclusive owners, and it's either a project or author.
           """
        aset = self.get_asset_set()
        if not aset:
            return
        return aset.is_editable()

    def get_asset_edit_url(self):
        """Return a URL pointing at the Add Asset page for this target's
           corresponding asset type, with the asset path already filled in.

           Assumes that is_editable() has already been checked.
           """
        return "/account/%ss/add/%s/" % self.path.split('/', 1)

    def enforce_defaults(self):
        if not self.title:
            self.title = self.get_default_title()
            self.save()

    def __unicode__(self):
        return smart_unicode(self.title or self.get_default_title())

    class Admin:
        pass

