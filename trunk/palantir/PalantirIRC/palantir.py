''' palantir.py

This module implements features for palantir such as nick-completion and
message formatting.

  Copyright (C) 2004 W. Evan Sheehan <evan@navi.cx>
'''

import string, re
from time import localtime

def formatMessage(user, msg, isAction=False, ourName=None):
  ''' Format a message for displaying. Returns a tuple of the time,
      a string and a boolean value; the string is the nick of the
      person who spoke plus the message and the boolean indicates
      whether or not the our nick was said.'''
  if ourName and msg.find(ourName) != -1 and user.find(ourName) != -1:
    addressed = True
  else:
    addressed = False

  if isAction:
    nick = ' * ' + getNick(user, isAction) + ' '
  elif user:
    nick = ' <' + getNick(user, isAction) + '> '
  else:
    nick = ''
  return (getTime(), nick + msg, addressed)

def formatRoll(user, times, sides, rolls, total):
  ''' Returns a tuple with the time and a formatted string of the die
      roll data formatted for display.
      '''
  return (getTime(),
       getNick(user) + ' rolled a ' + times + 'd' + sides + ': => ' + total)

def formatCTCP(user, to, message):
  ''' Format a CTCP message and return it in a tuple along with the time. '''
  print message
  return (None, None)

def getNick(user, isAction):
  ''' Return the nickname from the user name passed as the argument. '''
  return re.search('([^!]*).*', user).group(1)

def getTime():
  ''' Return the local hour, minute and seconds. '''
  time = localtime()

  hour = str(time[3])
  if len(hour) is 1: hour = '0'+hour

  min = str(time[4])
  if len(min) is 1: min = '0'+min

  sec = str(time[5])
  if len(sec) is 1: sec = '0'+sec

  return (hour, min, sec)

def nickComplete(text, nicks):
  ''' Try and complete text from the list nicks.  Returns a tuple where
      the first item is the longest matching substring in all the nicks
      and every subsequent item is a possible completion.  If there is a
      complete match the tuple have only the matching item in it.
      '''
  match = [text]
  for nick in nicks:
    if nick.find(text) != -1:
      match.append(nick)

  return match
