#
# Models for compatibility with the LibCIA database.
# These are only used by conversion scripts for data migration.
#

from django.db import models

class AuditTrail(models.Model):
    id = models.IntegerField()
    timestamp = models.IntegerField()
    uid = models.IntegerField()
    action_domain = models.CharField(maxlength=96)
    action_name = models.TextField()
    main_param = models.TextField(blank=True)
    params = models.TextField(blank=True)
    allowed = models.IntegerField()
    results = models.TextField(blank=True)
    class Meta:
        db_table = 'audit_trail'

class Users(models.Model):
    uid = models.IntegerField(primary_key=True)
    secret_key = models.CharField(maxlength=96)
    active = models.IntegerField()
    full_name = models.TextField(blank=True)
    email = models.TextField(blank=True)
    creation_time = models.IntegerField()
    key_atime = models.IntegerField(null=True, blank=True)
    login_name = models.CharField(blank=True, maxlength=96)
    login_passwd_md5 = models.CharField(blank=True, maxlength=96)
    login_atime = models.IntegerField(null=True, blank=True)
    login_mtime = models.IntegerField(null=True, blank=True)
    class Meta:
        db_table = 'users'

class Capabilities(models.Model):
    uid = models.ForeignKey(Users, db_column='uid')
    cap_md5 = models.CharField(maxlength=96)
    cap_repr = models.TextField()
    class Meta:
        db_table = 'capabilities'

class Meta(models.Model):
    name = models.CharField(blank=True, maxlength=96)
    value = models.CharField(blank=True, maxlength=765)
    class Meta:
        db_table = 'meta'

class Rulesets(models.Model):
    uri = models.TextField()
    xml = models.TextField()
    class Meta:
        db_table = 'rulesets'

class StatsCatalog(models.Model):
    parent = models.ForeignKey('self', null=True, blank=True, db_column='parent_path')
    path = models.CharField(primary_key=True, maxlength=384, db_column='target_path')
    class Meta:
        db_table = 'stats_catalog'

class StatsCounters(models.Model):
    target = models.ForeignKey(StatsCatalog, db_column='target_path', primary_key=True)
    name = models.CharField(maxlength=96)
    event_count = models.IntegerField()
    first_time = models.IntegerField(null=True, blank=True)
    last_time = models.IntegerField(null=True, blank=True)
    class Meta:
        db_table = 'stats_counters'

class StatsMetadata(models.Model):
    target = models.ForeignKey(StatsCatalog, db_column='target_path',
                               primary_key=True, related_name='metadata')
    name = models.CharField(maxlength=96)
    mime_type = models.CharField(maxlength=96)
    value = models.TextField()
    mtime = models.IntegerField(null=True, blank=True)
    class Meta:
        db_table = 'stats_metadata'

class StatsRelations(models.Model):
    target_a_path = models.ForeignKey(StatsCatalog, db_column='target_a_path', primary_key=True)
    target_b_path = models.ForeignKey(StatsCatalog, db_column='target_b_path', primary_key=True)
    strength = models.IntegerField()
    freshness = models.IntegerField(null=True, blank=True)
    class Meta:
        db_table = 'stats_relations'

class StatsSubscriptions(models.Model):
    id = models.IntegerField(primary_key=True)
    target_path = models.CharField(maxlength=384)
    expiration = models.IntegerField()
    scope = models.CharField(blank=True, maxlength=96)
    client = models.CharField(blank=True, maxlength=192)
    trigger = models.TextField()
    failures = models.IntegerField()
    class Meta:
        db_table = 'stats_subscriptions'

