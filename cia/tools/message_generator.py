#!/usr/bin/env python
"""
Submit randomly timed random messages to CIA.
Unlike torture_stats, which uses random messages to
stress-test and profile CIA, this simulates a slightly
more normal level of activity.
"""

import Client, RandomMessage
import time, random

class MessageGenerator(Client.App):
    def main(self):
        while True:
            time.sleep(random.uniform(0, 10))
            self.server.hub.deliver(RandomMessage.generate())

if __name__ == '__main__':
    MessageGenerator().main()

### The End ###
