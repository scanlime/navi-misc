from django.conf import settings
from django.forms import PasswordField, TextField
from django.utils.html import escape

class DojoTextField(TextField):
    def __init__(self, field_name, length=30, maxlength=None, is_required=False, validator_list=None, member_name=None):
        TextField.__init__(self, field_name, length, maxlength, is_required, validator_list, member_name)

    def render(self, data):
        if data is None:
            data = ''

        maxlength = ''
        if self.maxlength:
            maxlength = 'maxlength="%s" ' % self.maxlength

        if isinstance(data, unicode):
            data = data.encode(settings.DEFAULT_CHARSET)

        return '<input type="text" id="%s" class="dojo-ValidationTextbox" name="%s" size="%s" value="%s" %s dojo:required="%s" />' % \
            (self.get_id(), self.field_name, self.length, escape(data), maxlength, self.is_required)

class DojoPasswordField(PasswordField):
    def __init__(self, field_name, length=30, maxlength=None, is_required=False, validator_list=None, member_name=None):
        PasswordField.__init__(self, field_name, length, maxlength, is_required, validator_list, member_name)

    def render(self, data):
        if data is None:
            data = ''

        maxlength = ''
        if self.maxlength:
            maxlength = 'maxlength="%s" ' % self.maxlength

        if isinstance(data, unicode):
            data = data.encode(settings.DEFAULT_CHARSET)

        return '<input type="password" id="%s" class="dojo-ValidationTextbox" name="%s" size="%s" value="%s" %s dojo:required="%s" />' % \
            (self.get_id(), self.field_name, self.length, escape(data), maxlength, self.is_required)
