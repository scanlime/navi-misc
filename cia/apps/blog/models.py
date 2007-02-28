from django.db import models
from docutils.core import publish_parts
from django.contrib.auth.models import User
from django.core.cache import cache

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

    def render(self):
        key = 'cia.apps.blog.%s.%s' % (self.slug, str(self.pub_date).replace(' ','_'))
        parts = cache.get(key)
        if parts:
            return parts

        parts = publish_parts(
            source = self.content,
            writer_name = "html4css1",
            settings_overrides = {
                'cloak_email_addresses': True,
                'initial_header_level': 2,
                },
            )
        cache.set(key, parts)
        return parts

    class Admin:
        pass
