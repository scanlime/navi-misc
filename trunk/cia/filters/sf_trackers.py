#!/usr/bin/python
import sys, email, smtplib, re
from StringIO import StringIO

returnAddress = "sf_trackers@picogui.org"
toAddress = "commits@picogui.org"

message = email.message_from_file(sys.stdin)
body = StringIO(message.get_payload())

m = re.search('\[.*\]', message['subject'])
namestring = m.string[m.start():m.end()][2:-2]
(project, tracker, id) = namestring.split('-')
title = m.string[m.end() + 1:]

log = ""

while True:
  line = body.readline().strip()
  if not line:
    break
  if re.match('^Message', line):
    m = re.search('\(.*\)', line)
    type = m.string[m.start():m.end()][1:-1]
    user = line.split(' ')[-1:][0]

def GetChangedSettings(body):
  retval = ''
  while True:
    line = body.readline().strip()
    if not line:
      break
    if re.match('^>', line):
      category = line.split(' ')[0][1:-1]
      change = " ".join(line.split(' ')[1:])
      retval += " %s->'%s'" % (category, change)
  return retval

def GetSubmitter(body):
  while True:
    line = body.readline().strip()
    if not line:
      break
    if re.match('^Submitted By:', line):
      m = re.search('\(.*\)', line)
      submitter = m.string[m.start():m.end()][1:-1]
      return submitter

def GetInitialComment(body):
  # comments should start with a new line to put the comments on their own block
  retval = '\n'
  while True:
    line = body.readline().strip()
    if not line:
      break
    if re.match('^Initial Comment:', line):
      while True:
        line = body.readline().strip()
	if line == '----------------------------------------------------------------------':
	  return retval
	retval += line + '\n'

def GetNewComment(body):
  # comments should start with a new line to put the comments on their own block
  retval = '\n'
  state = 0
  while True:
    line = body.readline().strip()
    if not line:
      break
    if state == 0 and re.match('^>Comment By', line):
      state = 1
    if state == 1 and re.match('^user_id', line):
      state = 2
    if state == 2:
      line = body.readline().strip()
      while True:
	line = body.readline().strip()
	if line == '----------------------------------------------------------------------':
	  return retval
	retval += line + '\n'

verb = ''
if type == 'Tracker Item Submitted':
  verb = 'opened'
  if user == 'Submitter':
    user = GetSubmitter(body)
  log += GetInitialComment(body)
elif type == 'Settings changed':
  verb = 'changed'
  log += GetChangedSettings(body)
elif type == 'Comment added':
  verb = 'changed'
  log += GetChangedSettings(body) + GetNewComment(body)
else:
  # something else? add it here
  pass

body = "%s %s %s %s (%s): %s" % (user, verb, tracker, id, title, log)

s = smtplib.SMTP()
s.connect()
s.sendmail(returnAddress, toAddress,
           "From: %s\nTo: %s\nSubject: SendToChannels %s\n\n%s" % \
           (returnAddress, toAddress, project, body))
