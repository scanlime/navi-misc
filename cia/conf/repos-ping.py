#!/usr/bin/env python
#
# Repository pinger script: reads an incoming "ping" mail from stdin,
# locates the matching repository, pings the repository, and optionally
# forwards the mail.
#
#   DJANGO_SETTINGS_MODULE=cia.settings PYTHONPATH=~/ ~/cia/conf/repos-ping.py < message
#

from cia.apps.repos.models import Repository
import sys, re, email

def get_repository_for_message(msg):
    ping_re = re.compile(r"ping\+([a-zA-Z0-9]+)@")
    match = ping_re.search(msg['to']) or ping_re.search(msg['x-original-to'])
    if match:
        name = match.group(1)
        try:
            return Repository.objects.get(is_active=True, pinger_name=name)
        except Repository.DoesNotExist:
            pass

def main():
    msg = email.message_from_file(sys.stdin)
    repos = get_repository_for_message(msg)
    if not repos:
        return

    repos.get_client().poll()

    if repos.forward_pinger_mail:
        from cia.apps.accounts.authplus import send_mail_to_user
        from cia.apps.accounts.models import Project, UserAsset
        from django.contrib.contenttypes.models import ContentType

        # Forward pinger mail to all project owners
        project = Project.objects.get(repos=repos)
        ct = ContentType.objects.get_for_model(Project)
        for user_asset in UserAsset.objects.filter(content_type=ct, object_id=project.id):

            send_mail_to_user(user_asset.user, "repos/pinger-fwd.txt", msg['from'],
                              msg=msg, repos=repos, user_asset=user_asset)

if __name__ == '__main__':
    main()
