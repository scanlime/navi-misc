from django.db import models
from django.conf import settings
from django.contrib.auth.models import User
import random

class REPOS_TYPE:
    SUBVERSION = 1

repos_type_choices = (
    (REPOS_TYPE.SUBVERSION, 'Subversion'),
    )

yes_no_choices = (
    (False, 'No'),
    (True,  'Yes'),
    )

class RepositoryManager(models.Manager):
    _pinger_alphabet = "acdefghjklmnpqrtuvwxyz"
    _pinger_length = 8
    
    def get_new_pinger_name(self):
        """Return a random pinger name that isn't being used.  This
           name is not security-critical, but we'll make it hard to
           guess in order to prevent accidental or intentional
           triggering of other project's pollers. This can help
           prevent the use of repository pingers to DoS someone else's
           Subversion repository.
           """
        while 1:            
            name = ''.join([random.choice(self._pinger_alphabet) for i in range(self._pinger_length)])
            try:
                self.get(pinger_name=name)
            except self.model.DoesNotExist:
                break
        return name

class Repository(models.Model):
    objects = RepositoryManager()
    is_active = models.BooleanField('Use repository', default=True, choices=yes_no_choices)

    # Repository identity
    type = models.PositiveSmallIntegerField(choices=repos_type_choices, default=REPOS_TYPE.SUBVERSION)
    location = models.CharField("Repository location", maxlength=255, blank=False)

    # Optional polling
    enable_polling = models.BooleanField(default=False, choices=yes_no_choices)
    poll_frequency = models.PositiveIntegerField("Polling frequency in minutes", default=15)

    # Optional e-mail pinger
    pinger_name = models.CharField(maxlength=64, db_index=True)
    forward_pinger_mail = models.BooleanField(default=False, choices=yes_no_choices)

    # Who owns this repository?
    project_name = models.CharField(maxlength=128, db_index=True, blank=False)
    created_by = models.ForeignKey(User, null=True)

    # Other project configuration
    default_module_name = models.CharField(maxlength=64, null=True)
    path_regexes = models.TextField("Path regular expressions", null=True)
    revision_url = models.CharField("Revision URL pattern", maxlength=255, null=True)

    # Repository state
    root_url = models.CharField("Repository root URL", maxlength=255, null=True)
    uuid = models.CharField("Repository UUID", maxlength=255, null=True)
    last_revision = models.PositiveIntegerField(default=0, null=True)
    last_update_time = models.DateTimeField(null=True)

    def __str__(self):
        return "%s repository at %s" % (self.get_type_display(), self.location)

    def get_pinger_email(self):
        return "ping-%s@%s" % (self.pinger_name, settings.CIA_INCOMING_MAIL_DOMAIN)

    class Admin:
        pass
