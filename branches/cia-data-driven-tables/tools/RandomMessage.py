""" RandomMessage

A simple random commit message generator, for simulating
input when testing CIA. This is just a module providing
the message generator itself, other tools in this directory
make use of the random messages.
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

import random, time


randomAuthor = (
    ('jim', 'captain_', 'flux', 'mr', 'sr', 'lurgy', 'mj', 'kr', 'zx',
     'agent_', 'death_', 'monkey_', 'super_', 'talkie', 'waffle',
     'squid', 'slinky', 'ensign_', 'professor_', 'liquid_', 'larry',
     '', '', '', ''),
    ('proton', 'fry', 'neo', 'bender', 'diablo', 'muffin', 'wibble',
     'zork', 'bob', 'smith', 'chuck', 'bologna', 'cheese', 'guru', 'leper',
     'duck', 'yam', 'squid', 'zoidberg', 'guido', 'spielberg', 'torvalds',
     'tigert'),
    ('', '', '', '', '', '42', '3', '999'),
    )

randomProject = (
    ('py', 'c', 'x', 'g', 'k', 'lib', ''),
    ('widgets', 'desktop', 'mouse', 'snail', 'vacuum', 'squeegie', 'dog', 'squiggle'),
    ('', '', '', '', '', '++', '2', '3', '-enhanced'),
    )

randomLog = (
    ('Update ', 'Frobnicate ', 'Break ', 'Explode ', 'Revert ', 'Test ', 'Rewrite '),
    ('all ', 'the ', 'a few ', '', '', '', ''),
    ('recent ', 'old ', 'tasty ', '', '', ''),
    ('bits ', 'files ', 'classes ', 'squirrels '),
    ('in ', 'in ', 'in ', 'near '),
    ('the ',),
    ('database ', 'network ', 'operating system ', 'lego ', 'graphics ', 'build system '),
    ('module', 'package', 'subsystem'),
    )

def fromList(l):
    """Generate a random message my combining random strings chosen
       from the provided sequence of sequences of strings.
       """
    return ''.join([random.choice(choices) for choices in l])

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
    """ % (fromList(randomProject),
           fromList(randomAuthor),
           fromList(randomLog),
	   rev)

### The End ###
