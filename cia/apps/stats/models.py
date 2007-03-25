from django.db import models
from urllib import quote
from cia.apps.images.models import ImageSource, ImageInstance
from django.newforms.util import smart_unicode, StrAndUnicode

class StatsTargetManager(models.Manager):
    def withIcons(self, size):
        """Return a query that includes icon information.
           Icons are disabled if 'size' is None.
           """
        if not size:
            return self.all()

        #
        # XXX: It's kind of gross to have three sub-selects here,
        #      but I was having trouble getting the proper semantics
        #      for targets with no icon by just using 'tables' and 'where'.
        #
        iconMatch = ('thumbnail_size = %d AND '
                     'source_id = '
                     'IF(stats_statstarget.icon_id IS NOT NULL, '
                     'stats_statstarget.icon_id, stats_statstarget.photo_id)') % size

        return self.extra(
            select = {
                'icon_path': 'SELECT path FROM images_imageinstance WHERE ' + iconMatch,
                'icon_width': 'SELECT width FROM images_imageinstance WHERE ' + iconMatch,
                'icon_height': 'SELECT height FROM images_imageinstance WHERE ' + iconMatch,
                },
            )

class StatsTarget(StrAndUnicode, models.Model):
    """Refers to stats stored by the stats subsystem. This is a
       location in the database where information about a project,
       author, etc. are stored.

       Right now these stats paths are intimately tied to stats
       metadata and to the actual URL used by a stats page.
       Eventually this table will become a reference to a stored
       Esquilax query along with an individual URL.
       """
    objects = StatsTargetManager()
    
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

    def get_query_icon(self):
        """Retrieve an ImageInstance() for the icon that was retrieved by withIcons()"""
        if getattr(self, 'icon_path', None):
            return ImageInstance(path=self.icon_path, width=self.icon_width, height=self.icon_height)

    def get_default_title(self):
        title = self.path.rsplit('/', 1)[-1]
        if title:
            return title[0].upper() + title[1:]
        else:
            # Default name for the root page
            return "Stats"

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

    def get_absolute_url(self):
        return "/stats/" + quote(self.path)

    def get_asset_edit_url(self):
        """Return a URL pointing at the Add Asset page for this target's
           corresponding asset type, with the asset path already filled in.

           Assumes that is_editable() has already been checked.
           """
        return "/account/%ss/add/%s/" % tuple(self.path.split('/', 1))

    def enforce_defaults(self):
        if not self.title:
            self.title = self.get_default_title()
            self.save()

    def __unicode__(self):
        return smart_unicode(self.title or self.get_default_title())

    class Admin:
        pass

