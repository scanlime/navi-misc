#!/usr/bin/env python
#
# Convert stats metadata from the old DB's format to the Django site's format.
#
# DJANGO_SETTINGS_MODULE=cia.settings PYTHONPATH=~/ ~/cia/tools/django_convert_metadata.py
#

from django.db import models
import Image
from cStringIO import StringIO
from cia.apps.stats.models import StatsTarget
from cia.apps.legacy.models import StatsMetadata
from cia.apps.images.models import ImageInstance

def convert_metadata():
    for metadata in StatsMetadata.objects.all():
        new_target = StatsTarget.objects.get_or_create(path = metadata.target.path)[0]
        print metadata.target.path, metadata.name

        # Text fields
        if metadata.name in ('title', 'subtitle', 'url', 'description',
                             'links-filter', 'related-filter'):
            setattr(new_target, metadata.name.replace('-', '_'), metadata.value.tostring().strip())

        # Image fields
        if metadata.name in ('icon', 'photo'):
            try:
                im = Image.open(StringIO(metadata.value))
            except IOError:
                print "Corrupt %s for %r" % (metadata.name, metadata.target.path)
            else:
                image = ImageInstance.objects.create_original(
                    im, created_by=None, is_temporary=False)
                setattr(new_target, metadata.name, image)

        new_target.save()

if __name__ == "__main__":
    convert_metadata()
