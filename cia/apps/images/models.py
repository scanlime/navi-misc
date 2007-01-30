from django.db import models
from django.contrib.auth.models import User
from django.conf import settings
import os, re, Image

# These must be listed from largest to smallest
THUMBNAIL_SIZES = (256, 128, 64, 32, 16)


class Source(models.Model):
    """This table provides each immutable image with a unique ID
       number, and it provides metadata about the source of that
       image. Every image with the same 'Source' should be identical
       aside from format or resolution differences.
       """
    is_temporary = models.BooleanField(default=True)
    reviewed_by_admin = models.BooleanField(default=False)
    created_by = models.ForeignKey(User, null=True)
    date_added = models.DateTimeField(auto_now_add=True)

    def create_path(self, suffix, extension=".png"):
        id_dirs = "/".join(re.findall("..?", "%x" % self.id))
        return "".join((id_dirs, suffix, extension))

    def get_thumbnail(self, size=128):
        return self.instances.get(thumbnail_size = size)

    def get_original(self):
        return self.instances.get(is_original = True)


class InstanceManager(models.Manager):
    def create_from_image(self, image, source, suffix="", **kw):
        """Create an image instance from a PIL image, using
           the provided path suffix.
           """
        path = source.create_path(suffix)
        i = Instance(source = source,
                     path = path,
                     width = image.size[0],
                     height = image.size[1],
                     **kw)
        i.store_image(image)
        i.save()

    def create_original(self, image, created_by):
        """Create an original image from uploaded data, given a PIL
           Image object. The resulting image will always be saved as
           a PNG file. Automatically creates all default thumbnail sizes.

           Returns the new Source instance.

           Warning: The input image is modified!
           """
        source = Source.objects.create(created_by = created_by)
        self.create_from_image(image, source, is_original=True)

        for size in THUMBNAIL_SIZES:
            self.create_thumbnail(image, source, size)

        return source

    def create_thumbnail(self, image, source, size):
        """Create a thumbnail at the specified size.
           Modifies the source image! This can be used
           to efficiently create thumbnails at several
           sizes with little quality loss, as long as
           larger sizes come first.
           """
        image.thumbnail((size, size), Image.ANTIALIAS)
        return self.create_from_image(image, source, "-t%d" % size, thumbnail_size=size)


class Instance(models.Model):
    """An image file representing a source image in a particular
       size. The size is cached. The image itself is stored in the CIA
       flat-file database, and served by the static file server.
       """
    objects = InstanceManager()

    source = models.ForeignKey(Source, related_name='instances')

    is_original = models.BooleanField(default=False)
    thumbnail_size = models.PositiveIntegerField(null=True, blank=True)

    path = models.CharField(maxlength=32)
    width = models.PositiveIntegerField()
    height = models.PositiveIntegerField()

    def get_url(self):
        return '/images/db/' + self.path

    def get_path(self):
        return os.path.join(settings.CIA_DATA_PATH, 'db', 'images' , self.path)

    def store_image(self, im):
        full_path = self.get_path()
        directory = os.path.dirname(full_path)
        try:
            os.makedirs(directory)
        except OSError:
            pass
        im.save(full_path)
