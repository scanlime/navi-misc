#!/usr/bin/env python
"""
Creates stats:// rulesets and sends a large number of messages
as fast as possible to populate the stats database. This is
intended for evaluating the scalability of CIA's database, and
profiling the speed of CIA's message delivery system.
"""

import Client, RandomMessage
import time


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
        messages = [RandomMessage.generate(rev=i) for i in xrange(10000)]
        startTime = time.time()
        for i, message in enumerate(messages):
            speed = i / (time.time() - startTime)
            print "Message %d/%d...\taverage %.02f messages/second" % (i, len(messages), speed)
            self.server.hub.deliver(message)

if __name__ == '__main__':
    TortureStats().main()

### The End ###
