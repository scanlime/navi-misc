""" Menu.py

Some preliminary excursions into the world of Python. ^^
Creates a menu out of the dictionary menuItems and returns the
function call that the user selects.
"""
#
# W. Evan Sheehan
# evan@navi.picogui.org
#

import sys

def menu(title, menuItems):
  """ Create a menu, print it, and retrieve a value from the user
  'title' is the menu title, 'menuItems' is a dictionary of the
  menu items where the keys are displayed in the menu and the values
  are function names. """

  print title
  itemNum = 0
  itemDict = {}

  # print menu and set up dictionary for selection.
  for item, value in menuItems.iteritems():
    itemNum += 1
    print "%d. %s" % (itemNum, item)
    itemDict[itemNum] = value

  print "Enter a number:"

  # Error Handling.
  try:
    choice = int(sys.stdin.readline())
    return itemDict[choice]
  except ValueError:
    print "Please enter only numbers."
  except KeyError:
    print "Please enter a number from the menu."
