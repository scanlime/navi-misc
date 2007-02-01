#!/usr/bin/env python
#
# Remove thumbnails and all other non-original images.
#
# DJANGO_SETTINGS_MODULE=cia.settings PYTHONPATH=~/ ~/cia/tools/images_remove_thumbs.py
#

from cia.apps.images.models import ImageInstance

ImageInstance.objects.filter(is_original=False).delete()
