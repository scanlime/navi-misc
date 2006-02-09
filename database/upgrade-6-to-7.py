#
# Upgrades a CIA database from version 6 to version 7.
#
# Version 7 of the database marked the removal of the
# stats_messages table, moving all messages into a custom
# ring buffer format. This script will read in stats_messages
# and generate the new flat files according to the default
# paths in the LibCIA.Files module.
#
# Back up your database before running this!
#
# Note that this process isn't fully automatic. If the
# conversion is successful, you'll want to manually bump
# the database version to 7, and remove the stats_messages
# table. Since InnoDB won't reclaim the space, you may
# wish to dump the whole database, delete the innodb table
# space, then reload the database. This isn't nearly as
# bad as it sounds, since the db is quite reasonably
# sized without stats_messages.
#
# If the conversion takes a while and you want to pick
# up the commits you missed while the conversion was taking
# place, note the ID of the last converted message and add
# a "WHERE id > foo" clause to the stats_messages query.
#
# -- Micah Dowty
#

import sys, os
sys.path[0] = os.path.join(sys.path[0], "..")
from LibCIA import Database, XML
from LibCIA.Stats.Target import StatsTarget

Database.init(serverCursor=True)
cursor = Database.pool.connect().cursor()

# Make sure we're starting with version 3
cursor.execute("SELECT value FROM meta WHERE name = 'version'")
if cursor.fetchall()[0][0] != "6":
    raise Exception("This script must only be run on version 6 databases")

# To avoid spending all our time reading buffer headers, cache frequently used targets
targetCache = {}
targetHits = {}
targetCacheMax = 128

rowsProcessed = 0
prevId = 0

cursor.execute("SELECT * FROM stats_messages")
while True:
    row = cursor.fetchone()
    if not row:
        break

    target_path, db_id, timestamp, xml = row

    # Make sure our messages aren't going back in time
    assert db_id > prevId
    prevId = db_id

    if target_path in targetCache:
        # Cache hit
        target = targetCache[target_path]
        targetHits[target_path] += 1
    else:
        # We don't have enough file descriptors (and maybe RAM) to keep all
        # targets open. Drop the least popular entries from our cache to add this one.
        while len(targetCache) > targetCacheMax:
            mru = [(hits, path) for path, hits in targetHits.iteritems() if path in targetCache]
            mru.sort()
            del targetCache[mru[0][1]]

        target = StatsTarget(target_path)
        targetCache[target_path] = target
        targetHits[target_path] = 1

    try:
        target.messages.push(xml)
    except XML.ParseException:
        print "Parse error: message %d for %r" % (db_id, target_path)

    rowsProcessed += 1
    if (rowsProcessed % 1000) == 0:
        print "%d messages converted" % rowsProcessed

print "Done with conversion!"

#print "Deleting v6 message database..."
#cursor.execute("DROP TABLE stats_messages")

# Seems this was a success, bump the db version
#cursor.execute("UPDATE meta SET value = '7' WHERE name = 'version'")

### The End ###
