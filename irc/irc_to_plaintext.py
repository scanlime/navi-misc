#!/usr/bin/env python
import sys, re

class LogFormatter:
   def formatLine(self, line):
      timestamp, message = line.split(" ", 1)

      # Strip colors
      message = re.sub("(\x03..|\x0F|\x02)", "", message)

      if message[0] == '<':
         # Normal conversation
         sender, content = message[1:].split("> ", 1)
         print "<%s> %s" % (self.formatNick(sender), content.strip())

      elif message[0] == '[':
         # A CTCP message
         sender, ctcp = message[1:].split("] ", 1)
         msgType, content = ctcp.split(" ", 1)
         if msgType == "ACTION":
            print "* %s %s" % (self.formatNick(sender), content.strip())

   def formatNick(self, spec):
      return spec.split("!",1)[0].strip()

if __name__ == '__main__':
   f = LogFormatter()
   while 1:
      f.formatLine(sys.stdin.readline())
      sys.stdout.flush()

### The End ###
