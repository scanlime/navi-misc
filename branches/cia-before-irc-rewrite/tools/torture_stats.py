#!/usr/bin/env python
"""
Creates stats:// rulesets and sends a large number of messages to
populate the stats database. This is intended for evaluating the
scalability of CIA's database.
"""

import Client
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

def generateRandom(l):
    """Generate a random message my combining random strings chosen
       from the provided sequence of sequences of strings.
       """
    return ''.join([random.choice(choices) for choices in l])

def randomCommit(rev=1):
    """Create a random commit message"""
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
    """ % (generateRandom(randomProject),
           generateRandom(randomAuthor),
           generateRandom(randomLog),
	   rev)


class TortureStats(Client.App):
    rulesets = [
"""\
<ruleset uri='stats://author'>
    <return path='/message/body/commit/author'/>
</ruleset>
""","""\
<ruleset uri='stats://project'>
    <return path='/message/source/project'/>
</ruleset>
""","""\
<ruleset uri='stats://all'>
    <return>.</return>
</ruleset>
"""]

    def main(self):
        # Load rulesets
        print "Loading rulesets..."
        for ruleset in self.rulesets:
            self.server.ruleset.store(self.key, ruleset)

        # Deliver random messages. We generate them beforehand,
        # so the time that takes isn't included in the simple benchmark
        print "Generating messages..."
        messages = [randomCommit(i) for i in xrange(10000)]
        startTime = time.time()
        for i, message in enumerate(messages):
            speed = i / (time.time() - startTime)
            print "Message %d/%d...\taverage %.02f messages/second" % (i, len(messages), speed)
            self.server.hub.deliver(message)

if __name__ == '__main__':
    TortureStats().main()

### The End ###
