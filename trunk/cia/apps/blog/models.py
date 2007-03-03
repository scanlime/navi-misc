from django.db import models
from django.contrib.auth.models import User
from django.core.cache import cache
from docutils.core import publish_doctree, Publisher
from docutils.readers import doctree
from docutils.io import DocTreeInput, StringOutput
from docutils.nodes import SparseNodeVisitor
from cia.apps.images.models import ImageSource, ImageInstance
import re


class ImageTranslator(SparseNodeVisitor):
    """This Docutils translator implements support for a new type of
       image element with a numeric URL, referring to an image managed
       by our cia.apps.images app. Examples:

       A full-size image:

          .. image:: #123

       The default thumbnail:

          .. image:: #123/

       A 64-pixel thumbnail:

          .. image:: #123/64


       A 256-pixel thumbnail, floating in a box:

          .. image:: #123/256
             :class: float

       """
    def __init__(self, document):
        SparseNodeVisitor.__init__(self, document)
        self.images = []
    
    def visit_image(self, node):
        m = re.match(r"^#(\d+)(/(\d*))?$", node['uri'])
        if not m:
            return
        image_id, use_thumbnail, thumbnail_size = m.groups()

        try:
            image = ImageSource.objects.get(pk=image_id)
        except ImageSource.DoesNotExist:
            err = self.document.reporter.error("Image ID %s does not exist" % image_id)
            node.replace_self(err)
            return

        try:
            if use_thumbnail:
                if thumbnail_size:
                    instance = image.get_thumbnail(int(thumbnail_size))
                else:
                    instance = image.get_thumbnail()
            else:
                instance = image.get_original()
        except ImageInstance.DoesNotExist:
            err = self.document.reporter.error("Image not available in the specified size")
            node.replace_self(err)
            return
            
        self.images.append(image)

        node['uri'] = instance.get_url()
        node['width'] = str(instance.width)
        node['height'] = str(instance.height)


class Post(models.Model):
    slug = models.SlugField('slug', unique_for_date='pub_date',
                            prepopulate_from=['title'], db_index=True)
    pub_date = models.DateTimeField(db_index=True)
    posted_by = models.ForeignKey(User)
    listed = models.BooleanField('Listed in public indexes?', default=False)
    title = models.CharField(maxlength=100)
    content = models.TextField()

    def __str__(self):
        return '"%s" posted by %s at %s' % (self.title, self.posted_by, self.pub_date)

    def get_absolute_url(self):
        return '/blog/%04d/%02d/%s/' % (self.pub_date.year, self.pub_date.month, self.slug)

    def invalidate_cache(self):
        cache.delete('cia.apps.blog.%d' % self.id)

    def reference_images(self):
        """Find all images used by this blog post, and reference them
           permanently. This prevents the images from being treated
           as temporary, and automatically discarded.
           """
        document = publish_doctree(source = self.content)
        visitor = ImageTranslator(document)
        document.walkabout(visitor)
        for image in visitor.images:
            image.reference()

    def render(self):
        key = 'cia.apps.blog.%d' % self.id
        parts = cache.get(key)
        if not parts:

            # Convert the reST markup to a document tree
            document = publish_doctree(source = self.content)

            visitor = ImageTranslator(document)
            document.walkabout(visitor)

            #
            # Publish that document tree as HTML.  We can't use any of
            # the simpler methods in docutils.core, since we need
            # access to writer.parts
            #
            reader = doctree.Reader(parser_name='null')
            pub = Publisher(reader, None, None,
                            source = DocTreeInput(document),
                            destination_class = StringOutput)
            pub.set_writer('html4css1')
            pub.process_programmatic_settings(None, {
                'cloak_email_addresses': True,
                'initial_header_level': 2,
                }, None)

            pub.publish()
            parts = pub.writer.parts

            cache.set(key, parts)
        return parts

    class Admin:
        pass
