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
  if ourName and msg.find(ourName) != -1 and user.find(ourName) == -1:
    addressed = True
  else:
    addressed = False

  if isAction:
    nick = ' * ' + getNick(user) + ' '
  elif user:
    nick = ' <' + getNick(user) + '> '
  else:
    nick = ''
  return (getTime(), nick, msg, addressed)

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

def getNick(user):
  ''' Return the nickname from the user name passed as the argument. '''
  regex = re.search('([^!]*).*', user)
  if regex:
    nick = regex.group(1)
  else:
    nick = user
  return nick

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

  # Separate out the search string from the rest of the sentence.
  search = text.split()[len(text.split())-1]

  # Insert any nicks containing the substring text into the matched list.
  for nick in nicks:
    if nick.find(search) != -1:
      match.append(nick)

  # If there's only one match in the list replace the search string with the
  # match.
  if len(match) == 2:
    match[0] = match[0].replace(search, match[1], 1)
    match.remove(match[1])
  else:
    match[0] = match[0].replace(search, findCommonSubstring(match[1:]), 1)

  # Return the list of matches.
  return match

def findCommonSubstring(strings):
  substr = strings[0]
  for string in strings:
    for index in range(len(string)):
      if substr[index] != string[index]:
	substr = string[:index]
	break
  return substr
