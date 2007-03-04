#!/usr/bin/env python
#
# Generate new thumbnails on all images. We remove old thumbnails as we go.
#
# DJANGO_SETTINGS_MODULE=cia.settings PYTHONPATH=~/ ~/cia/tools/images_generate_thumbs.py
#

from cia.apps.images.models import ImageInstance, ImageSource

for source in ImageSource.objects.all():
    print source
    ImageInstance.objects.filter(source=source, is_original=False).delete()
    ImageInstance.objects.create_standard_thumbnails(source, source.get_original())
