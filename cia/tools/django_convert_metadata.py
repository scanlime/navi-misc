#!/usr/bin/env python
#
# Convert stats metadata from the old DB's format to the Django site's format.
#
# DJANGO_SETTINGS_MODULE=cia.settings PYTHONPATH=~/ ~/cia/tools/django_convert_metadata.py
#

from django.db import models
import Image
from cStringIO import StringIO
from cia.apps.accounts.models import StatsTarget
from cia.apps.legacy.models import StatsMetadata
from cia.apps.images.models import ImageInstance

def convert_metadata():
    for metadata in StatsMetadata.objects.all():
        new_target = StatsTarget.objects.get_or_create(path = metadata.target.path)[0]
        print metadata.target.path, metadata.name

        if metadata.name == 'title':
            new_target.title = metadata.value

        if metadata.name == 'subtitle':
            new_target.subtitle = metadata.value

        if metadata.name == 'url':
            new_target.url = metadata.value

        if metadata.name == 'description':
            new_target.description = metadata.value

        if metadata.name == 'links-filter':
            new_target.links_filter = metadata.value

        if metadata.name == 'related-filter':
            new_target.related_filter = metadata.value

        if metadata.name == 'icon':
            try:
                im = Image.open(StringIO(metadata.value))
            except IOError:
                print "Corrupt icon for %r" % metadata.name
            else:
                new_target.icon = ImageInstance.objects.create_original(
                    im, created_by=None, is_temporary=False)

        if metadata.name == 'photo':
            try:
                im = Image.open(StringIO(metadata.value))
            except IOError:
                print "Corrupt photo for %r" % metadata.name
            else:
                new_target.photo = ImageInstance.objects.create_original(
                    im, created_by=None, is_temporary=False)

        new_target.save()

if __name__ == "__main__":
    convert_metadata()
