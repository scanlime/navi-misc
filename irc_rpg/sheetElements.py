""" sheetElements.py

Classes of each element in the character sheet.

Copyright (C) 2003 W. Evan Sheehan <evan@navi.picogui.org>
"""

import string


class character_sheet:
  """ Super class for layout objects of a character sheet. """
  def __init__(self):
    self.children = []
    self.attributes = {}

  def setAttributes(self, node):
    """ Get the attributes of node and store it in a dictionary. """
    for i in range(node.attributes.length):
      self.attributes[node.attributes.item(i).name] = node.getAttribute(node.attributes.item(i).name)

  def addChild(self, newChild):
    """ Add the new child to the children list if it isn't None. """
    if newChild is not None:
      self.children.append(newChild)

class tab_view(character_sheet):
  """ Contains the tabs in the character sheet. """

class tab(character_sheet):
  """ Information on each individual tag. """

class text_field(character_sheet):
  """ Generic text field. """

class dice(character_sheet):
  """ Dice widgets can be contained in the character sheet for frequently
      made rolls.
      """
  def __init__(self):
    character_sheet.__init__(self)
    self.times = 0

  def addData(self, data):
    """ Increase the number of times to roll this die. """
    print data.strip()
