#!/usr/bin/env python
#
# Periodic cleanup job for CIA's database.
# This can be run from cron to purge old data.
#
# DJANGO_SETTINGS_MODULE=cia.settings PYTHONPATH=~/ ~/cia/tools/db_cleanup.py 
#

from django.contrib.sessions.models import Session
from cia.apps.images.models import ImageSource
import datetime

def clean_up():
    # Remove all expired sessions

    Session.objects.filter(
        expire_date__lt = datetime.datetime.now(),
        ).delete()

    # Remove temporary images older than a week.
    # This automatically removes image instances and their files on disk.
    
    ImageSource.objects.filter(
        is_temporary = True,
        date_added__lt = datetime.datetime.now() - datetime.timedelta(days=7),
        ).delete()

if __name__ == "__main__":
    clean_up()
