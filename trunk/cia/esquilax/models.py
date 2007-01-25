from django.db import models


class Atom(models.Model):
    name = models.CharField(maxlength=128, db_index=True, unique=True)

class Message(models.Model):
    file_id = models.PositiveIntegerField()
    offset = models.PositiveIntegerField()

    project_index = models.ForeignKey(Atom, related_name='project_messages')
    module_index = models.ForeignKey(Atom, related_name='module_messages')
    author_index = models.ForeignKey(Atom, related_name='author_messages')

esquilax_indexes = {
    'project_index': '/message/source/project',
    'module_index': '/message/source/module',
    'author_index': '/message/body/commit/author',
    }

# Create your models here.
