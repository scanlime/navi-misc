#!/usr/bin/env python
#
# Generate new thumbnails on all images. We remove old thumbnails as we go.
#
# DJANGO_SETTINGS_MODULE=cia.settings PYTHONPATH=~/ ~/cia/tools/images_generate_thumbs.py
#

from cia.apps.images.models import ImageInstance, ImageSource
import Image

for source in ImageSource.objects.all():
    print source
    ImageInstance.objects.filter(source=source, is_original=False).delete()
    im = Image.open(source.get_original().get_path())
    ImageInstance.objects.create_standard_thumbnails(im, source)
