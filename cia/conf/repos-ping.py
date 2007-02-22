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
    match = re.search(r"ping\+([a-zA-Z0-9]+)@", msg['to'])
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

        # Try to look up the UserAsset for this repository's project, so
        # we can give the user a direct link back to the account page for
        # disabling this email.
        try:
            project = Project.objects.get(repos=repos)
            user_asset = Project.objects.all_for_user(repos.created_by).get(object_id=project.id)
        except (Project.DoesNotExist, UserAsset.DoesNotExist):
            user_asset = None
        
        send_mail_to_user(repos.created_by, "repos/pinger-fwd.txt", msg['from'],
                          msg=msg, repos=repos, user_asset=user_asset)

if __name__ == '__main__':
    main()
