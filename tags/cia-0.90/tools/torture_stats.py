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

        RandomMessage.benchmark(self.server)

if __name__ == '__main__':
    TortureStats().main()

### The End ###
