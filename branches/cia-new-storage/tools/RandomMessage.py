""" RandomMessage

A simple random commit message generator, for simulating
input when testing CIA. This is just a module providing
the message generator itself, other tools in this directory
make use of the random messages.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2005 Micah Dowty <micah@navi.cx>
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

import random, time


class RandomList:
    """A list of items that are serialized to a string
       by randomly selecting one item to serialize.
       """
    def __init__(self, *l):
        self.list = l

    def __str__(self):
        return str(random.choice(self.list))

class JoinedList(list):
    """A list of items that are serializable to a string
       by serializing all child objects and joining them.
       """
    def __init__(self, *l):
        self.list = l

    def __str__(self):
        return ''.join(map(str, self.list))

class Optional:
    """A class that, when serialized, randomly chooses whether
       or not to include its argument. If not, it returns the empty string.
       """
    def __init__(self, arg, probability):
        self.arg = arg
        self.probability = probability

    def __str__(self):
        if random.random() <= self.probability:
            return str(self.arg)
        else:
            return ''

class RandomInt:
    """Serializes to a ranom integer"""
    def __init__(self, lower, upper):
        self.lower = lower
        self.upper = upper

    def __str__(self):
        return str(random.randint(self.lower, self.upper))


class RandomRepeat:
    """Repeats and joins the argument a random number of times"""
    def __init__(self, arg, lower, upper, separator=''):
        self.arg = arg
        self.lower = lower
        self.upper = upper
        self.separator = separator

    def __str__(self):
        return self.separator.join(map(str, [self.arg]*random.randint(self.lower, self.upper)))


randomAuthor = JoinedList(
    RandomList('jim', 'captain_', 'flux', 'mr', 'sr', 'lurgy', 'mj', 'kr', 'zx',
     'agent_', 'death_', 'monkey_', 'super_', 'talkie', 'waffle',
     'squid', 'slinky', 'ensign_', 'professor_', 'liquid_', 'larry',
               '', '', '', ''),
    RandomList('proton', 'fry', 'neo', 'bender', 'diablo', 'muffin', 'wibble',
               'zork', 'bob', 'smith', 'chuck', 'bologna', 'cheese', 'guru', 'leper',
               'duck', 'yam', 'squid', 'zoidberg', 'guido', 'spielberg', 'torvalds',
               'tigert'),
    RandomList('', '', '', '', '', '42', '3', '999'),
    )

randomProject = JoinedList(
    RandomList('py', 'c', 'x', 'g', 'k', 'lib', ''),
    RandomList('widgets', 'desktop', 'mouse', 'snail', 'vacuum', 'squeegie', 'dog', 'squiggle'),
    RandomList('', '', '', '', '', '++', '2', '3', '-enhanced'),
    )

randomLog = JoinedList(
    Optional(RandomList('oops... ', 'Scratch that. ', 'Maybe it would help if I added this file correctly... ',
                        'Wow, a duck. ', 'Good thing I was wearing my bullet-proof socks. '),
             0.1),
    RandomRepeat(RandomList(JoinedList(RandomList('Updated ', 'Rewrote ', 'Fixed ', 'Retested '),
                                       RandomList('this', 'everything', 'this module', 'the makefile'),
                                       Optional(' yet again', 0.2),
                                       Optional(' for the third time', 0.2),
                                       Optional(' like a fox', 0.2),
                                       Optional(JoinedList(' this ',
                                                           RandomList('week', 'month', 'day', 'hour', 'year')),
                                                0.1),
                                       '.',
                                       ),
                            JoinedList(RandomList('Update ', 'Frobnicate ', 'Break ', 'Explode ', 'Revert ', 'Test ', 'Rewrite '),
                                       RandomList('all ', 'the ', 'a few ', '', '', '', ''),
                                       RandomList('recent ', 'old ', 'tasty ', '', '', ''),
                                       RandomList('bits ', 'files ', 'classes ', 'squirrels '),
                                       RandomList('in ', 'in ', 'in ', 'near '),
                                       RandomList('the ',),
                                       RandomList('database ', 'network ', 'operating system ', 'lego ', 'graphics ', 'build system '),
                                       RandomList('module', 'package', 'subsystem'),
                                       '.',
                                       ),
                            ),
                 1, 6, ' '),
    Optional(" Let's hope it works.", 0.02),
    Optional(JoinedList(" Cleaning up after ", randomAuthor, "."), 0.1),
    Optional(JoinedList(" Let's hope ", randomAuthor, " doesn't notice."), 0.1),
    Optional(JoinedList(" Closes bug #", RandomInt(100, 100000)),
             0.4),
    )


def generate(rev=None):
    """Create a random commit message"""
    # The revision can be specified, or we can make a random one
    if rev is None:
        rev = random.randint(1, 4000)

    return """
    <message>
        <generator><name>torture_stats.py</name></generator>
        <source><project>%s</project></source>
        <body>
            <commit>
                <author>%s</author>
                <log>%s</log>
                <revision>%s</revision>
            </commit>
        </body>
    </message>
    """ % (randomProject,
           randomAuthor,
           randomLog,
           rev)


def benchmark(server, numMessages=5000, verbose=True):
    """Deliver many random messages, and time their delivery to the server.
       Returns the average speed in messages per second.
       """
    if verbose:
        print "Generating messages..."
    messages = [generate(rev=i) for i in xrange(numMessages)]
    startTime = time.time()
    for i, message in enumerate(messages):
        server.hub.deliver(message)
        speed = (i+1) / (time.time() - startTime)
        if verbose and not i % 50:
            print "Message %d/%d...\taverage %.02f messages/second" % (i, len(messages), speed)
    return speed

### The End ###
