from django.shortcuts import render_to_response
from django.template.context import RequestContext
from cia.apps.stats.models import StatsTarget

from LibCIA.Stats.Target import StatsTarget as OldStatsTarget
from LibCIA import Formatters as OldFormatters
from LibCIA.Message import Message as OldMessage
from Nouvelle.Serial import Serializer

class Message:
    serializer = Serializer()

    def __init__(self, (id, xml)):
        self.id = id
        self.hex_id = "%x" % id
        self.oldmsg = OldMessage(xml)

    def to_html(self):
        f = OldFormatters.getFactory()
        nouvelleTree = f.findMedium('xhtml', self.oldmsg).formatMessage(self.oldmsg)
        return self.serializer.render(nouvelleTree)

def stats_page(request, path):
    oldtarget = OldStatsTarget(path)
    try:
        target = StatsTarget.objects.get(path=path)
    except StatsTarget.DoesNotExist:
        # If we don't have a stats target in the db, create a temp
        target = StatsTarget(path=path)

    messages = map(Message, oldtarget.messages.getLatest(20))
    messages.reverse()

    return render_to_response('stats/stats_page.html', RequestContext(request, {
        'path': path,
        'target': target,
        'recent_messages': messages,
        }))
