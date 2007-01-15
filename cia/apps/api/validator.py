import django.newforms as forms
from cia.apps.api.util import json_result

def validation_result(view_func):
    def _wrap(request, *args, **kwargs):
        try:
            view_func(request, *args, **kwargs)
            return {
                'is_valid': True,
                'messages': [],
                }
        except forms.ValidationError, e:
            return {
                'is_valid': False,
                'messages': e.messages,
                }

    _wrap.__doc__ = view_func.__doc__
    _wrap.__dict__ = view_func.__dict__
    return _wrap

@json_result
@validation_result
def ruleset(request):
    from cia.apps.accounts.models import validate_ruleset
    validate_ruleset(request.POST.get('content'))
