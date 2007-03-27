from django.shortcuts import render_to_response
from django.template.context import RequestContext
from django.utils.html import escape
from cia.apps.api.util import json_result
from cia.apps.stats.models import StatsTarget
from cia.apps.legacy.models import StatsCatalog
import string

def removeDuplicates(list, key='url', filter=string.lower):
    l = []
    memo = set()
    for item in list:
        k = filter(item[key])
        if k not in memo:
            l.append(item)
            memo.add(k)
    return l

def statsTargetResult(target):
    if target.path.startswith('project/'):
        section = 'projects'
    elif target.path.startswith('author/'):
        section = 'authors'
    else:
        section = 'stats'

    return {
        'url': target.get_absolute_url(),
        'title': unicode(target),
        'section': section,
        }

@json_result
def search(request, limit=10):
    #
    # Prepare UTF-8, Unicode, and ASCII versions of the query
    #
    utf8Query = request.GET.get("q")
    if not utf8Query:
        return {}
    query = unicode(utf8Query, 'utf8')
    try:
        asciiQuery = query.encode('ascii', 'strict')
    except UnicodeEncodeError:
        asciiQuery = None

    # Other search parameters
    try:
        iconSize = int(request.GET.get("ico", 0)) or None
    except ValueError:
        iconSize = None

    results = []

    #
    # Search for stats targets by title
    #
    # This table supports Unicode, but Django's "contains" operator
    # doesn't handle Unicode properly. We need to use the encoded version.
    #
    # We'll also pick up icon information in the same query.
    #
    for target in StatsTarget.objects.withIcons(iconSize).filter(title__icontains = utf8Query)[:limit]:
        result = statsTargetResult(target)
        icon = target.get_query_icon()
        if icon:
            result['icon'] = {
                'url': icon.get_url(),
                'width': icon.width,
                'height': icon.height,
                }
        results.append(result)

    #
    # Search for stats targets by path
    #
    # This legacy table does not support Unicode- skip this search if
    # the query can't be represented in ASCII.
    #
    # Note that removeDuplicates() will cause the above entries
    # (with proper metadata from the database) to override these
    # if we match both.
    #
    if asciiQuery:
        for catalog in StatsCatalog.objects.filter(path__icontains = asciiQuery)[:limit]:
            results.append(statsTargetResult(StatsTarget(path=catalog.path)))
            
    return {
        'results': removeDuplicates(results),
        }
