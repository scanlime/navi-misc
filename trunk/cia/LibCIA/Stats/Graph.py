""" LibCIA.Stats.Graph

Maintains an undirected graph of associations between stats targets.
These associations are reinforced when one message is delivered to
multiple targets.

Using this graph stored in the database, various methods of visualization
have been implemented here.
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

from LibCIA import Database
import time


class Relation:
    """Represents a relationship between two stats targets- an edge on the stats graph.
       These relationships can be queried or reinforced. You can ask a stats target for
       a list of relations containing it.
       """
    def __init__(self, a, b):
        # Our targets must be sorted by path, to make this edge unique
        if a.path > b.path:
            a, b = b, a
        self.a = a
        self.b = b

    def reinforce(self):
        """Increment this relation's strength and set its freshness to the current time"""
        return Database.pool.runInteraction(self._reinforce)

    def _reinforce(self, cursor):
        """Database interaction implementing reinforce()"""
        # First touch the edge to make sure it exists. We have to do this
        # inside two autoCreateTargetFor levels, to create both stats targets
        # if they don't yet exist.
        self.a._autoCreateTargetFor(cursor, self.b._autoCreateTargetFor,
                                    cursor, cursor.execute,
                                    "INSERT IGNORE INTO stats_relations "
                                    "(target_a_path, target_b_path) VALUES(%s, %s)" %
                                    (Database.quote(self.a.path, 'varchar'),
                                     Database.quote(self.b.path, 'varchar')))

        cursor.execute("UPDATE stats_relations "
                       "SET strength = strength + 1, freshness = %s "
                       "WHERE target_a_path = %s AND target_b_path = %s" %
                       (Database.quote(int(time.time()), 'bigint'),
                        Database.quote(self.a.path, 'varchar'),
                        Database.quote(self.b.path, 'varchar')))

### The End ###
