#
# fields.py -- Model fields.
#
# Copyright (c) 2007-2008  Christian Hammond
# Copyright (c) 2007-2008  David Trowbridge
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import base64
from datetime import datetime

from django.conf import settings
from django.core.serializers.json import DjangoJSONEncoder
from django.db import models
from django.dispatch import dispatcher
from django.utils import simplejson
from django.utils.encoding import smart_unicode


class Base64DecodedValue(str):
    """
    A subclass of string that can be identified by Base64Field, in order
    to prevent double-encoding or double-decoding.
    """
    pass


class Base64FieldCreator(object):
    def __init__(self, field):
        self.field = field

    def __set__(self, obj, value):
        pk_val = obj._get_pk_val(obj.__class__._meta)
        pk_set = pk_val is not None and smart_unicode(pk_val) != u''

        if (isinstance(value, Base64DecodedValue) or not pk_set):
            obj.__dict__[self.field.name] = base64.encodestring(value)
        else:
            obj.__dict__[self.field.name] = value

        setattr(obj, "%s_initted" % self.field.name, True)

    def __get__(self, obj, type=None):
        if obj is None:
            raise AttributeError('Can only be accessed via an instance.')

        value = obj.__dict__[self.field.name]

        if value is None:
            return None
        else:
            return Base64DecodedValue(base64.decodestring(value))


class Base64Field(models.TextField):
    """
    A subclass of TextField that encodes its data as base64 in the database.
    This is useful if you're dealing with unknown encodings and must guarantee
    that no modifications to the text occurs and that you can read/write
    the data in any database with any encoding.
    """
    def contribute_to_class(self, cls, name):
        super(Base64Field, self).contribute_to_class(cls, name)
        setattr(cls, self.name, Base64FieldCreator(self))

    def get_db_prep_save(self, value):
        if isinstance(value, Base64DecodedValue):
            value = base64.encodestring(value)

        return super(Base64Field, self).get_db_prep_save(value)

    def to_python(self, value):
        if isinstance(value, Base64DecodedValue):
            return value
        else:
            return Base64DecodedValue(base64.decodestring(value))


class ModificationTimestampField(models.DateTimeField):
    """
    A subclass of DateTimeField that only auto-updates the timestamp when
    updating an existing object or when the value of the field is None. This
    specialized field is equivalent to DateTimeField's auto_now=True, except
    it allows for custom timestamp values (needed for
    serialization/deserialization).
    """
    def __init__(self, verbose_name=None, name=None, **kwargs):
        kwargs.update({
            'editable': False,
            'blank': True,
        })
        models.DateTimeField.__init__(self, verbose_name, name, **kwargs)

    def pre_save(self, model, add):
        if not add or getattr(model, self.attname) is None:
            value = datetime.now()
            setattr(model, self.attname, value)
            return value

        return super(ModificationTimestampField, self).pre_save(model, add)

    def get_internal_type(self):
        return "DateTimeField"


class JSONField(models.TextField):
    """
    A field for storing JSON-encoded data. The data is accessible as standard
    Python data types and is transparently encoded/decoded to/from a JSON
    string in the database.
    """
    def __init__(self, verbose_name=None, name=None,
                 encoder=DjangoJSONEncoder(), **kwargs):
        models.TextField.__init__(self, verbose_name, name, blank=True,
                                  **kwargs)
        self.encoder = encoder

    def db_type(self):
        return "text"

    def contribute_to_class(self, cls, name):
        def get_json(model_instance):
            return self.dumps(getattr(model_instance, self.attname, None))

        def set_json(model_instance, json):
            setattr(instance, self.attname, self.loads(json))

        super(JSONField, self).contribute_to_class(cls, name)

        setattr(cls, "get_%s_json" % self.name, get_json)
        setattr(cls, "set_%s_json" % self.name, set_json)

        models.signals.post_init.connect(self.post_init, sender=cls)

    def pre_save(self, model_instance, add):
        return self.dumps(getattr(model_instance, self.attname, None))

    def post_init(self, instance=None):
        value = self.value_from_object(instance)

        if value:
            value = self.loads(value)
        else:
            value = {}

        setattr(instance, self.attname, value)

    def dumps(self, data):
        return self.encoder.encode(data)

    def loads(self, s):
        return simplejson.loads(s, encoding=settings.DEFAULT_CHARSET)
