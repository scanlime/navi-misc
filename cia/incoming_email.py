#!/usr/bin/env python
#
# incoming_email.py
#
# Process email directed at the CIA bot. For each incoming mail, this script
# is run with the message piped into stdin, as with most mail filtering scripts.
#
# This script just delivers the email to the local CIA server over XML-RPC
# and lets the server take care of logging the message and processing its contents.
#

import xmlrpclib, sys

message = sys.stdin.read()
s = xmlrpclib.ServerProxy("http://localhost:3910")
s.mail.deliver(message)

### The End ###
