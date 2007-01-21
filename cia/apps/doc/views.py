from django.shortcuts import render_to_response
from django.template.context import RequestContext
from django.http import Http404, HttpResponseRedirect
from django.conf import settings
from docutils.core import publish_parts
import os

def find_sidebar_path(path, format=".%s.sidebar"):
    dir, basename = os.path.split(path)
    specific = os.path.join(dir, format % basename)
    if os.path.isfile(specific):
        return specific
    else:
        return os.path.join(dir, format % 'default')

def parse_sidebar(path):
    """Parse sidebar links from a simple text file format.
       Lines beginning with a dash specify a new section heading,
       links are made by lines of the form 'title :: URL'.
       Other lines are ignored.

       Returns a list of sections. Each section is a dictionary
       with 'title' and 'links' keys. Each link is a dictionary
       with 'title' and 'url' keys.
       """
    sections = []
    for line in open(path).xreadlines():
        line = line.strip()
        if not line:
            continue

        if line[0] == '-':
            sections.append({
                'title': line[1:].strip(),
                'links': [],
                })
            continue

        pieces = line.split("::", 1)
        if len(pieces) == 2:
            title, url = pieces
            sections[-1]['links'].append({
                'title': title.strip(),
                'url': url.strip(),
                })
    return sections


def page(request, path):
    filePath = os.path.join(settings.CIA_DOC_PATH, path)

    # "index" is the document representing its parent
    # directory. We canonicalize URLs here such that they
    # never include "index".
    #
    if os.path.basename(filePath) == 'index':
        return HttpResponseRedirect("..")
    if os.path.isdir(filePath):
        filePath = os.path.join(filePath, 'index')
    if not os.path.isfile(filePath):
        raise Http404

    return render_to_response('layout_doc.html', RequestContext(request, {
        'parts': publish_parts(
            source = open(filePath).read(),
            writer_name = "html4css1",
            settings_overrides = {
                'cloak_email_addresses': True,
                'initial_header_level': 2,
                },
            ),
        'sidebar': parse_sidebar(find_sidebar_path(filePath)),
        }))
