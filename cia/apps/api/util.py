from django.utils import simplejson
from django.http import HttpResponse

def json_result(view_func):
    def _encode(request, *args, **kwargs):
        object = view_func(request, *args, **kwargs)
        json = simplejson.dumps(object, sort_keys=True)
        return HttpResponse(json, mimetype='application/json')

    _encode.__doc__ = view_func.__doc__
    _encode.__dict__ = view_func.__dict__
    return _encode
