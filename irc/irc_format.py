#!/usr/bin/env python
#
# A simple formatter for dircproxy logs, using Nouvelle.
# Pass the log into stdin, the web page title on the command line.
#
# -- Micah
#
import sys, time
from Nouvelle import tag, xml, Serializer


class LogFormatter:
   """Reads an IRC log in dircproxy format, returning a Nouvelle tree
      with the IRC log rendered in XHTML.
      """
   def format(self, f):
      """Read an IRC log from the given file-like object, returning
         the corresponding formatted Nouvelle tree.
         """
      return tag('table')[[
                 tag('tr')[
                     self.formatLine(line),
                 ]
                 for line in f.xreadlines()
             ]]

   def formatLine(self, line):
      """Format one line from the IRC log, returning a (timestamp, nick, content) tuple"""
      timestamp, message = line.split(" ", 1)
      timestamp = self.formatTimestamp(int(timestamp[1:]))
      nick = ()

      if message[0] == '<':
         # Normal conversation
         sender, content = message[1:].split("> ", 1)
         nick = [" <", self.formatNick(sender), "> "]

      elif message[0] == '-':
         # A system message of some sort
         content = tag('span', _class="serverMessage")[ message[1:].split("- ", 1)[1] ]

      elif message[0] == '[':
         # A CTCP message
         sender, ctcp = message[1:].split("] ", 1)
         msgType, content = ctcp.split(" ", 1)
         if msgType == "ACTION":
            content = [self.formatNick(sender), " ", content]
         else:
            content = ["CTCP ", msgType, " ", content]

      else:
         content = message

      return [tag('td', _class="timestamp")[ timestamp ],
              tag('td', _class="nick")[ nick ],
              tag('td', _class="content")[ content ]]

   def formatNick(self, spec):
      """Return a formatted nickname, given a nick!user@host specification"""
      return tag('span', _class='nick')[ spec.split("!",1)[0] ]

   def formatTimestamp(self, t):
      """Given a UNIX-style time, return the formatted representation"""
      return tag('span', _class='timestamp')[
                 time.strftime("%F %H:%M:%S", time.localtime(t))]


stylesheet = """
body {
    margin: 1em;
}

span.nick {
    font-weight: bold;
}

td.nick {
    padding: 0em 0.5em 0em 2em;
    text-align: right;
    border-right: 1px solid #888;
}

td.content {
    padding: 0em 0em 0em 0.25em;
}

span.timestamp {
    color: #888;
}

span.serverMessage {
    color: #800;
}
"""


if __name__ == '__main__':
   print Serializer().render([
       xml('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" '
           '"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">\n'),

       tag('html', xmlns="http://www.w3.org/1999/xhtml")[
           tag('head')[
               tag('title')[ " ".join(sys.argv[1:]) ],
               tag('style', _type='text/css')[ stylesheet ],
               tag('meta', **{'http-equiv': 'refresh', 'content': 15}),
           ],
           tag('body')[
               LogFormatter().format(sys.stdin),
           ],
      ],
  ])

### The End ###
