#!/usr/bin/env python
import sys, email, smtplib
from StringIO import StringIO

returnAddress = "mythtv_commits@picogui.org"
toAddress = "commits@picogui.org"
projectName = "mythtv"
logFile = "/home/commits/mail.log"

message = email.message_from_file(sys.stdin)
body = StringIO(message.get_payload())

# If this appears to be a reply, ignore it
if message['subject'].strip().lower().find("re") == 0:
    sys.exit(0)

# Author is the fourth token of the second line
body.readline()
author = body.readline().strip().split(" ")[3]

# The body is the set of non-blank lines starting after the "Log Message" line
log = ""
while not body.readline().startswith("Log"):
    pass
while True:
    line = body.readline().strip()
    if not line:
        break
    log += line + "\n"

ciaMessage = "commit by {green}%s{normal}: %s" % (author, log)

s = smtplib.SMTP()
s.connect()
s.sendmail(returnAddress, toAddress,
           "From: %s\nTo: %s\nSubject: Announce %s\n\n%s" % \
           (returnAddress, toAddress, projectName, ciaMessage))
s.close()
