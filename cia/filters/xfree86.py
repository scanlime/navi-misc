#!/usr/bin/env python
import sys, email, smtplib
from StringIO import StringIO

returnAddress = "xfree86_commits@picogui.org"
toAddress = "commits@picogui.org"
projectName = "xfree86"
logFile = "/home/commits/mail.log"

message = email.message_from_file(sys.stdin)
body = StringIO(message.get_payload())

# If this appears to be a reply, ignore it
if message['subject'].strip().lower().find("re") == 0:
    sys.exit(0)

# Module name is the third token in the subject
modName = message['subject'].split(" ")[2]

# Use the from address as the author
author = message['from']

# The body is the set of non-blank lines starting after "Log Message:"
log = ""
while True:
    line = body.readline()
    if not line:
        break
    if line.strip() == "Log message:":
        break
while True:
    line = body.readline().strip()
    if not line:
        break
    log += line + "\n"

ciaMessage = "%s {green}%s{normal}: %s" % (modName, author, log)

s = smtplib.SMTP()
s.connect()
s.sendmail(returnAddress, toAddress,
           "From: %s\nTo: %s\nSubject: Announce %s\n\n%s" % \
           (returnAddress, toAddress, projectName, ciaMessage))
s.close()

