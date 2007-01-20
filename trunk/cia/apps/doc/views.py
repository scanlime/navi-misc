from django.shortcuts import render_to_response
from django.template.context import RequestContext
from django.conf import settings
import os

def page(request, path):
    f = open(os.path.join(settings.CIA_DOC_PATH, path))
    return render_to_response('layout_doc.html', RequestContext(request, {
        'page': f.read(),
        }))
