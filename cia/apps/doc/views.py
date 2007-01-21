from django.shortcuts import render_to_response
from django.template.context import RequestContext
from django.http import Http404
from django.conf import settings
from docutils.core import publish_parts
import os

def page(request, path):
    try:
        f = open(os.path.join(settings.CIA_DOC_PATH, path))
    except IOError:
        raise Http404

    return render_to_response('layout_doc.html', RequestContext(request, {
        'parts': publish_parts(
            source = f.read(),
            writer_name = "html4css1",
            settings_overrides = {
                'cloak_email_addresses': True,
                'initial_header_level': 2,
                },
            ),
        }))
