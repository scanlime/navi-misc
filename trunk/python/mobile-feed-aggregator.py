#
# A quick little feed aggregator that produces minibrowser-friendly output
# -- Micah Dowty
#

import feedparser, time, Nouvelle
import urllib, os, shelve
from Nouvelle import tag, place, xml, ModPython
from mod_python import apache

class Page(ModPython.Page):
    document = [
        xml('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" '
            '"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">'),
        tag('html', xmlns="http://www.w3.org/1999/xhtml")[
            tag('head')[
                tag('title')[ place("title") ],
            ],
            tag('body')[
                place("content"),
            ],
        ],
    ]

class Entry:
    def __init__(self, data, feed):
        self.feed = feed
        try:
            self.title = data.title
        except AttributeError:
            self.title = "Untitled"
        self.content = xml(unicode(data.summary).encode("utf-8"))
        self.date = time.mktime(data.modified_parsed)

        # Create a globally unique ID for this post
        # FIXME
        self.guid = str(hash(self.content))

    def __cmp__(self, other):
        """Reverse-temporal sort"""
        return other.date - self.date

class FeedCache:
    lifetime = 60 * 30
    def __init__(self, filename):
        self.data = shelve.open(filename)

        # Is it too old?
        now = time.time()
        if self.data.get("_ctime", 0) < (now - self.lifetime):
            self.data.clear()
        self.data["_ctime"] = now
        self.data.sync()

defaultFeedCache = FeedCache("/tmp/py-feed-cache.%d" % os.getuid())

class Feed:
    def __init__(self, name, url):
        self.name = name
        if url in defaultFeedCache.data:
            self.xml = defaultFeedCache.data[url]
        else:
            self.xml = feedparser.parse(urllib.urlopen(url).read())
            defaultFeedCache.data[url] = self.xml

def formatDate(t):
    local = time.localtime(t)
    now = time.localtime(time.time())
    timeString = "%02d:%02d" % (local.tm_hour, local.tm_min)
    if local.tm_year == now.tm_year:
        if local.tm_yday == now.tm_yday:
            dateString = "Today"
        elif local.tm_yday == now.tm_yday - 1:
            dateString = "Yesterday"
        elif now.tm_yday - local.tm_yday < 7:
            dateString = time.strftime("%A", local)
        else:
            dateString = time.strftime("%b %d", local)
    else:
        dateString = time.strftime("%b %d", local)

    return "%s %s" % (timeString, dateString)

class EntryPage(Page):
    def __init__(self, entries):
        self.entries = entries

    def render_title(self, context):
        e = self.entries[context['args']['id']]
        return e.title

    def render_content(self, context):
        e = self.entries[context['args']['id']]
        return [
            tag('b')[ place('title') ],
            tag('br'),
            tag('i')[ e.feed.name ], " (", formatDate(e.date), ") ",
            tag('hr'),
            tag('p')[ e.content ],
            ]

class Aggregator(Page):
    title = "Feed Aggregator"

    def __init__(self, urls):
        self.entries = []
        self.feeds = []
        self.entryMap = {}
        for familiarName, url in urls:
            feed = Feed(familiarName, url)
            self.feeds.append(feed)
            for entry in feed.xml.entries:
                e = Entry(entry, feed)
                self.entries.append(e)
                self.entryMap[e.guid] = e
        self.entries.sort()

    def render_content(self, context):
        return [
            tag('h1')[ "Latest Entries" ],
            [self.renderEntry(entry) for entry in self.entries[:20]],
            ]

    def renderEntry(self, entry):
        return [
            tag('hr'),
            tag('a', href="feeds.py/entry?id=%s" % entry.guid)[ entry.title ],
            tag('br'),
            tag('i')[ entry.feed.name ], " (", formatDate(entry.date), ") ",
            ]

aggregator = Aggregator([
    ("Christian", "http://chipx86.com/blog/index.rdf"),
    ("David",     "http://david.navi.cx/blog/?feed=rss2"),
    ("Jen",       "http://www.livejournal.com/users/zakzak/data/rss"),
    ("Kendra",    "http://www.livejournal.com/users/irisierendrache/data/rss"),
    ("Micah",     "http://www.livejournal.com/users/scanwidget/data/rss"),
    ])

index = aggregator.publish
entry = EntryPage(aggregator.entryMap).publish 
