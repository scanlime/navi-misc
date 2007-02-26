from django.shortcuts import render_to_response
from django.template.context import RequestContext
from cia.apps.stats.models import StatsTarget

# Legacy imports
from LibCIA.Stats.Target import StatsTarget as OldStatsTarget
from LibCIA import Formatters as OldFormatters
from LibCIA.Message import Message as OldMessage
from LibCIA.Message import FormatterArgs
from LibCIA.Formatters.Commit import CommitFormatter
from LibCIA import XML
from Nouvelle.Serial import Serializer

class Message:
    serializer = Serializer()
    formatter_factory = OldFormatters.getFactory()

    def __init__(self, (id, xml)):
        self.id = id
        self.hex_id = "%x" % id
        self.oldmsg = OldMessage(xml)
        self.formatter = self.formatter_factory.findMedium('xhtml', self.oldmsg)
        self.is_commit = isinstance(self.formatter, CommitFormatter)

        if self.is_commit:
            for shortcut, path in XML.pathShortcuts.items():
                doc = XML.XPath(path).queryObject(self.oldmsg)
                if doc:
                    setattr(self, shortcut, XML.shallowText(doc[0]))

    def to_html(self):
        """Format any message as XHTML, using the LibCIA formatters and
           Nouvelle serializer. This is used as a fallback for non-commit
           messages.
           """
        return self.serializer.render(self.formatter.formatMessage(self.oldmsg))

    def format_log(self):
        return self.serializer.render(self.formatter.component_log(None, FormatterArgs(self.oldmsg)))

    def format_files(self):
        return self.serializer.render(self.formatter.component_files(None, FormatterArgs(self.oldmsg)))


def stats_page(request, path):
    oldtarget = OldStatsTarget(path)
    try:
        target = StatsTarget.objects.get(path=path)
    except StatsTarget.DoesNotExist:
        # If we don't have a stats target in the db, create a temp
        target = StatsTarget(path=path)
    target.enforce_defaults()

    messages = map(Message, oldtarget.messages.getLatest(20))
    messages.reverse()

    return render_to_response('stats/stats_page.html', RequestContext(request, {
        'path': path,
        'target': target,
        'recent_messages': messages,
        }))
