""" LibCIA.Stats.Handler

The stats:// URI handler, the root of the stats XML-RPC interface,
and the maintenance system. All the usual entry points for the stats system.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from twisted.python import log
from twisted.internet import defer
from LibCIA import Ruleset, TimeUtil, RpcServer, Database
import time, posixpath


class StatsURIHandler(Ruleset.RegexURIHandler):
    """Handles stats:// URIs. A stats target is chosen,
       and the message is delivered to it.
       """
    scheme = 'stats'
    regex = r"^stats://(?P<path>([a-zA-Z0-9_-]+(/[a-zA-Z0-9_-]+)*)?)$"

    def __init__(self):
        self.lastMessage = None
        self.messageTargets = []

    def message(self, uri, message, content):
        """Appends 'content' to the path represented by the given URI
           and delivers a message to its associated stats target.

           This includes a bit of a hack for tracking associations
           between stats targets. We assume that messages are delivered
           one at a time- if we get a duplicate message (presumably to a
           different stats target) we add that stats target to the list of
           targets this message has been delivered to, and reinforce the
           new relations this forms.
           """
        path = posixpath.join(self.parseURI(uri)['path'], content)
        target = StatsTarget(path)
        target.deliver(message)

        if message == self.lastMessage:
            for prevTarget in self.messageTargets:
                Relation(prevTarget, target).reinforce()
        else:
            self.messageTargets = []
        self.messageTargets.append(target)
        self.lastMessage = message


class StatsInterface(RpcServer.Interface):
    """An XML-RPC interface used to query stats"""
    def __init__(self):
        RpcServer.Interface.__init__(self)
        self.putSubHandler('metadata', MetadataInterface())
        self.putSubHandler('subscribe', SubscriptionInterface())

    def xmlrpc_catalog(self, path=''):
        """Return a list of subdirectories within this stats path"""
        result = defer.Deferred()
        d = StatsTarget(path).catalog()
        d.addErrback(result.errback)
        d.addCallback(self._catalog, result)
        return result

    def _catalog(self, items, result):
        """Convert the returned catalog into target names and return them via
           the provided Deferred instance.
           """
        result.callback([target.name for target in items])

    def xmlrpc_getLatestMessages(self, path, limit=None):
        """Return 'limit' latest messages delivered to this stats target,
           or all available recent messages if 'limit' isn't specified.
           """
        return StatsTarget(path).messages.getLatest(limit)

    def xmlrpc_getCounterValues(self, path, name):
        """Returns a dictionary with current values for the given counter.
           Note that times are returned as UNIX-style seconds since
           the epoch in UTC.
           """
        return StatsTarget(path).counters.getCounter(name)

    def protected_clearTarget(self, path):
        """Deletes any data stored at a given stats target or in any of its subtargets"""
        log.msg("Clearing stats path %r" % path)
        return StatsTarget(path).clear()

    def caps_clearTarget(self, rpcPath, statsPath):
        """In addition to the usual capabilities, allow ('stats.path', path)"""
        return self.makeDefaultCaps(rpcPath) + [('stats.path', statsPath)]


class Maintenance:
    """This class performs periodic maintenance of the stats database, including
       counter rollover and removing old messages.
       """
    # Maximum number of messages to keep around for each stats target
    maxTargetMessages = 200

    def run(self):
        """Performs one stats maintenance cycle, returning a Deferred that
           yields None when the maintenance is complete.
           """
        return Database.pool.runInteraction(self._run)

    def _run(self, cursor):
        """Database interaction implementing the maintenance cycle"""
        self.checkRollovers(cursor)
        self.pruneSubscriptions(cursor)
        self.pruneTargets(cursor)

    def checkOneRollover(self, cursor, previous, current):
        """Check for rollovers in one pair of consecutive time intervals,
           like yesterday/today or lastMonth/thisMonth. This is meant to
           be run inside a database interaction.
           """
        # Delete counters that are too old to bother keeping at all
        cursor.execute("DELETE FROM stats_counters "
                       "WHERE (name = %s OR name = %s) "
                       "AND first_time < %s" %
                       (Database.quote(previous, 'varchar'),
                        Database.quote(current, 'varchar'),
                        Database.quote(long(TimeUtil.Interval(previous).getFirstTimestamp()), 'bigint')))

        # Roll over remaining counters that are too old for current
        # but still within the range of previous. Note that there is a
        # race condition in which this update will fail because a timer
        # has been incremented between it and the above DELETE. It could
        # be prevented by locking the table, but it's probably not worth
        # it. If the rollover fails this time, it will get another chance.
        cursor.execute("UPDATE stats_counters SET name = %s "
                       "WHERE name = %s "
                       "AND first_time < %s" %
                       (Database.quote(previous, 'varchar'),
                        Database.quote(current, 'varchar'),
                        Database.quote(long(TimeUtil.Interval(current).getFirstTimestamp()), 'bigint')))

    def checkRollovers(self, cursor):
        """Check all applicable counters for rollovers. This should
           be executed inside a database interaction.
           """
        self.checkOneRollover(cursor, 'yesterday', 'today')
        self.checkOneRollover(cursor, 'lastWeek', 'thisWeek')
        self.checkOneRollover(cursor, 'lastMonth', 'thisMonth')

    def pruneTargets(self, cursor):
        """Find all stats targets, running pruneTarget on each"""
        cursor.execute("SELECT target_path FROM stats_catalog")
        for row in cursor.fetchall():
            self.pruneTarget(cursor, row[0])
        cursor.execute("OPTIMIZE TABLE stats_messages")

    def pruneTarget(self, cursor, path):
        """Delete messages that are too old, from one particular stats target"""
        # Find the ID of the oldest message we want to keep
        cursor.execute("SELECT id FROM stats_messages WHERE target_path = %s "
                       "ORDER BY id DESC LIMIT 1 OFFSET %d" %
                       (Database.quote(path, 'varchar'),
                        self.maxTargetMessages))
        row = cursor.fetchone()
        if row:
            id = row[0]
            cursor.execute("DELETE FROM stats_messages WHERE target_path = %s AND id < %d" %
                           (Database.quote(path, 'varchar'), id))

    def pruneSubscriptions(self, cursor, maxFailures=3):
        """Delete subscriptions that have expired"""
        cursor.execute("DELETE FROM stats_subscriptions WHERE expiration < %s" %
                       Database.quote(int(time.time()), 'bigint'))

### The End ###
