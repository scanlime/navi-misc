#!/usr/bin/env python
"""
Submit randomly timed random messages to CIA.
Unlike torture_stats, which uses random messages to
stress-test and profile CIA, this simulates a slightly
more normal level of activity.
"""

import Client, RandomMessage
import time, random, socket

class MessageGenerator(Client.App):
    def main(self):
        while True:
            try:
                self.server.hub.deliver(RandomMessage.generate())
            except socket.error:
                pass
            time.sleep(random.uniform(0, 7))

if __name__ == '__main__':
    MessageGenerator().main()

### The End ###
