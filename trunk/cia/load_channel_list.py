#!/usr/bin/env python
#
# load_channel_list.py
#
# Read the 'channels.list' file created by the old CIA
# bot and load an appropriate list of IRC rulesets into
# the new CIA server.
#

import xmlrpclib, re
server = xmlrpclib.ServerProxy("http://localhost:3910")

for line in open('channels.list'):
    channel = "#" + line.strip()

    # Most channels had to be named after their project,
    # but it would also deliver to channels suffixed
    # with -commits
    project = re.sub("-commits$", "", line.strip())

    if channel in ('#commits', '#only-commits'):
        # The 'commits' and 'only-commits' channels would
        # display all projects, prefixed by the project name
        ruleset = "<ruleset>\n\t<formatter medium='irc'/>\n\t<formatter name='IRCProjectName'/>\n</ruleset>"

    else:
        # All other projects only displayed commits for the particular project
        ruleset = ("<ruleset>\n\t<match path='/message/source/project'>" +
                   project + "</match>\n\t<formatter medium='irc'/>\n</ruleset>")

    server.deliverMessage(
        "<message><body><ircRuleset channel=%r>%s</ircRuleset></body></message>" %
        (channel, ruleset))

### The End ###
