#!/usr/bin/env python
#
# Repository poller script: to be run once per minute from Cron.  This
# searches for active repositories with polling enabled, which are due
# to fire during this minute. Each matching repository is polled in
# parallel, using separate threads.
#
#   DJANGO_SETTINGS_MODULE=cia.settings PYTHONPATH=~/ ~/cia/conf/repos-poll.py
#
# XXX: This is a really naive implementation. Optimize it later...
#

from cia.apps.repos.models import Repository
import time, threading

class PollerThread(threading.Thread):
    def __init__(self, repo):
        self.repo = repo
        threading.Thread.__init__(self)

    def run(self):
        self.repo.get_client().poll()

def main():
    minute = time.time() // 60

    for repo in Repository.objects.filter(enable_polling=True, is_active=True):
        if repo.poll_frequency < 1 or minute % repo.poll_frequency == 0:
            PollerThread(repo).start()

if __name__ == '__main__':
    main()
