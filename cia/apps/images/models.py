from django.db import models
from django.contrib.auth.models import User
from django.conf import settings
from django.db.models import signals
from django.dispatch import dispatcher
import os, re, commands, random
from popen2 import Popen4

# These must be listed from largest to smallest
THUMBNAIL_SIZES = (256, 128, 64, 32, 16)


class ImageException(Exception):
    pass

class ImageMagick:
    """Simple interface to ImageMagick. Why ImageMagick? PIL's
       thumbnailing is very buggy, and I'm tired of working around
       those bugs. The most severe problems with PIL for me have been:

         - Lack of support for interlaced PNGs, buggy (or just confusing?)
           support for transparent GIFs.

         - A fraction of a pixel is cut off of the right and bottom sides
           of an image when thumbnailing. This can severely reduce the quality
           of our 16x16 and 32x32 icons.

       There is no actively maintained ImageMagick interface for Python-
       and even if there were, we are probably better off doing large image
       manipulation operations in a separate address space. This just calls
       out to ImageMagick using popen.
       """
    
    # Note the 'composite' option: Some input images are treated as
    # multi-image sequences. A common example is the Windows .ico format.
    #
    # The -background option here should give us transparent PNGs when rendering vector images.
    #
    CONVERT = "convert -background none -composite -quality 100"

    IDENTIFY = "identify -ping"

    def get_image_size(self, file_path):
        """Probe the size of an on-disk image, returning a (width, height) tuple.
           """
        child = Popen4("%s -ping %s" % (self.IDENTIFY, commands.mkarg(file_path)))
        output = child.fromchild.read()
        status = child.wait()
        match = re.search(r" (\d+)x(\d+) ", output)

        if status or not match:
            raise ImageException(output)

        return int(match.group(1)), int(match.group(2))

    def _get_temp_path(self, filename_hint=None):
        """Return a new path to a temporary file, using
           the provided filename as a hint for the temporary
           file's extension.
           """
        dir = os.path.join(settings.CIA_DATA_PATH, 'temp')
        try:
            os.makedirs(dir)
        except OSError:
            pass

        path = os.path.join(dir, "image-%d-%d" % (
            os.getpid(), random.randint(1000, 9999)))

        if filename_hint:
            m = re.search(r"(\.[a-z]+)$", filename_hint.lower())
            if m:
                path += m.group(1)

        return path

    def store_image(self, dest_path, data, filename_hint=None):
        """Given an in-memory image, store it to the provided
           path. This will validate the image and convert it to the
           file format indicated by the target path.  On error, no
           file will be written.

           Some formats cannot be reliably identified by
           ImageMagick. For these formats, the filename_hint may be
           necessary.
           """
        src_path = self._get_temp_path(filename_hint)
        try:
            # We could stream the data to ImageMagick over stdin rather
            # than writing it to a tempoary file, but:
            #
            #   1. ImageMagick just uses a temporary file internally anyway
            #
            #   2. Reading and writing the child process simultaneously,
            #      without introducing deadlocks, would significantly
            #      complicate this function.

            f = open(src_path, "wb")
            f.write(data)
            f.close()
            del data
            
            child = Popen4("%s %s %s" % (
                self.CONVERT,
                commands.mkarg(src_path),
                commands.mkarg(dest_path),
                ))

            output = child.fromchild.read()
            status = child.wait()
            if status:
                raise ImageException(output)

        finally:
            try:
                os.unlink(src_path)
            except OSError:
                pass

    def thumbnail_image(self, src_path, dest_path, size):
        """Generate a square thumbnail, with on-disk source and destination paths."""
        child = Popen4("%s -geometry %dx%d %s %s" % (
            self.CONVERT,
            size, size,
            commands.mkarg(src_path),
            commands.mkarg(dest_path),
            ))

        output = child.fromchild.read()
        status = child.wait()
        if status:
            raise ImageException(output)


class ImageSource(models.Model):
    """This table provides each immutable image with a unique ID
       number, and it provides metadata about the source of that
       image. Every image with the same ImageSource should be identical
       aside from format or resolution differences.
       """
    is_temporary = models.BooleanField(default=True)
    reviewed_by_admin = models.BooleanField(default=False)
    created_by = models.ForeignKey(User, null=True)
    date_added = models.DateTimeField(auto_now_add=True)

    def create_path(self, suffix="", extension=".png"):
        id_dirs = "/".join(re.findall("..?", "%x" % self.id))
        return "".join((id_dirs, suffix, extension))

    def get_thumbnail(self, size=128):
        return self.instances.get(thumbnail_size = size)

    def get_large_thumbnail(self):
        """Convenience method for templates. If we need more than one
           or two sizes, this should turn into a template filter."""
        return self.get_thumbnail(256)

    def get_original(self):
        return self.instances.get(is_original = True)

    def reference(self):
        """Inform this image that it's been referenced permanently.
           This clears the is_temporary flag and saves, if necessary.
           """
        if self.is_temporary:
            self.is_temporary = False
            self.save()

    def to_html(self):
        # Currently this is used only by the change history browser.
        return '<a href="%s">%s</a>' % (
            self.get_original().get_url(),
            self.get_thumbnail(size=64).to_html(),
            )

    def __str__(self):
        original = self.get_original()
        s = "Image #%d (%dx%d)" % (self.id, original.width, original.height)
        if self.is_temporary:
            s += " (temp)"
        return s

class ImageInstanceManager(models.Manager):
    magick = ImageMagick()

    def create_from_file(self, image, source, suffix="", **kw):
        """Create an image instance, after the image file has
           already been created on disk.
           """

    def create_original(self, image, created_by, is_temporary=True):
        """Create an original image from uploaded data, in the form of
           a FILES dictionary entry. The resulting image will always
           be saved as a PNG file. Automatically creates all default
           thumbnail sizes.

           Returns the new ImageSource instance on success. If the
           image is bad, raises ImageException.
           """

        # Must create the ImageSource first, since our path is based on its ID.
        source = ImageSource.objects.create(created_by = created_by,
                                            is_temporary = is_temporary)

        original = ImageInstance(source = source,
                                 path = source.create_path(),
                                 is_original = True)

        try:
            self.magick.store_image(original.get_path(create=True),
                                    image['content'], image.get('filename'))
        except:
            source.delete()
            raise

        original.update_image_size()
        original.save()

        self.create_standard_thumbnails(source, original)
        return source

    def create_standard_thumbnails(self, source, original):
        for size in THUMBNAIL_SIZES:
            self.create_thumbnail(source, original, size)

    def create_thumbnail(self, source, original, size):
        """Create a thumbnail at the specified size."""

        # If the image is already small enough, we can skip the
        # actual thumbnailing stage and just link back to the
        # original image.
        if max(original.width, original.height) <= size:
            return ImageInstance.objects.create(source = source,
                                                path = original.path,
                                                width = original.width,
                                                height = original.height,
                                                delete_file = False,
                                                thumbnail_size = size)

        thumb = ImageInstance(source = source,
                              path = source.create_path("-t%d" % size),
                              thumbnail_size = size)

        self.magick.thumbnail_image(original.get_path(), thumb.get_path(), size)

        thumb.update_image_size()
        thumb.save()
                                    

class ImageInstance(models.Model):
    """An image file representing a source image in a particular
       size. The size is cached. The image itself is stored in the CIA
       flat-file database, and served by the static file server.
       """
    objects = ImageInstanceManager()

    source = models.ForeignKey(ImageSource, related_name='instances')

    is_original = models.BooleanField(default=False)
    thumbnail_size = models.PositiveIntegerField(null=True, blank=True)

    path = models.CharField(maxlength=32)
    delete_file = models.BooleanField(default=True)

    width = models.PositiveIntegerField()
    height = models.PositiveIntegerField()

    def get_url(self):
        return '/images/db/' + self.path

    def get_path(self, create=False):
        full_path = os.path.join(settings.CIA_DATA_PATH, 'db', 'images' , self.path)
        if create:
            directory = os.path.dirname(full_path)
            try:
                os.makedirs(directory)
            except OSError:
                pass
        return full_path

    def update_image_size(self):
        self.width, self.height = ImageInstance.objects.magick.get_image_size(self.get_path())

    def to_html(self):
        return '<img src="%s" width="%d" height="%d" />' % (
            self.get_url(), self.width, self.height)

def remove_deleted_instance(instance):
    if instance.delete_file:
        try:
            os.unlink(instance.get_path())
        except OSError:
            pass

dispatcher.connect(remove_deleted_instance, signal=signals.post_delete, sender=ImageInstance)
