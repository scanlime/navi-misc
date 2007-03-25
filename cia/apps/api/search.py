from django.shortcuts import render_to_response
from django.template.context import RequestContext
from cia.apps.stats.models import StatsTarget
from cia.apps.api.util import json_result
from django.utils.html import escape

@json_result
def search(request):
    query = request.GET.get("q")

    results = []
    
    for target in StatsTarget.objects.filter(title__icontains = query)[:10]:
        results.append({
            'url': target.get_absolute_url(),
            'title': unicode(target),
            })

    return {'results': results}
