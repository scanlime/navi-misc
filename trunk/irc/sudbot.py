#! /usr/bin/env python

import string, random
from ircbot import SingleServerIRCBot
from irclib import nm_to_n, irc_lower

class TestBot(SingleServerIRCBot):
    def __init__(self, nickname, server, port=6667):
        SingleServerIRCBot.__init__(self, [(server, port)], nickname, nickname)
        self.start()

    def on_welcome(self, c, e):
#        c.join("#bzflag")
#        c.join("#picogui")
        c.join("#crystalspace")


    def on_pubmsg(self, c, e):
        for target in [
            "doug",
            "file",
	    "creeperz",
	    "cell",
	    "mikeeusa",
            ]:
            if string.find(string.lower(e._source),target) != -1:
                if random.random() < 0.2:
                    self.sud(c, e._target, target)
                    return
                for i in [
                    "slick",
                    "kewl",
                    "cool",
                    "31337",
                    "leet",
                    "fix",
                    "bug",
                    "bzflag",
                    "tracking",
                    "xandros",
                    "kiwi",
                    "picogui",
                    "crys",
		    "zip",
		    "ignore",
		    "BYE",
		    "fuck",
                    ]:
                    if string.find(string.lower(e._arguments[0]),i) != -1:
                        self.sud(c, e._target, target)
                        return

    def sud(self, c, channel, target):
        c.action(channel, "up %s" % target)
                
def main():
    server = "irc.freenode.net"
    nickname = "shut"

    bot = TestBot(nickname, server)
    bot.start()

if __name__ == "__main__":
    main()
