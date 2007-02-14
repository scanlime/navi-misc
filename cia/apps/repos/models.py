from django.db import models
from django.contrib.auth.models import User

class REPOS_TYPE:
    SUBVERSION = 1

repos_type_choices = (
    (REPOS_TYPE.SUBVERSION, 'Subversion'),
    )

class Repository(models.Model):

    # Repository identity
    type = models.PositiveSmallIntegerField(choices=repos_type_choices)
    location = models.CharField("Repository location", maxlength=255, blank=False)

    # Optional polling
    enable_polling = models.BooleanField(default=False)
    poll_frequency = models.PositiveIntegerField("Polling frequency in minutes", default=15)

    # Optional e-mail pinger
    pinger_name = models.CharField(maxlength=64, null=True, db_index=True)
    forward_pinger_mail = models.BooleanField(default=False)

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

    class Admin:
        pass
