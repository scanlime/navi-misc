''' GtkSheet.py

This module provides classes for turning an XML character sheet into gtk
widgets for a usable interface.

Copyright (C) 2003 W. Evan Sheehan <evan@navi.picogui.org>
'''

import xml.dom, xml.dom.minidom, string
from Character import Character


class Sheet:
  ''' Assemble the character sheet from the specified XML file using the
      objects in GtkSheetElements.
      '''
  def __init__(self, characterData, actionObject, elementModule):
    # Record the data for the character.
    self.character = characterData

    # The action object will handle the button presses in the character sheet.
    # This way the actions are more easily customizable depending on how the sheet
    # is opened.
    self.actionObject = actionObject

    self.elements = elementModule
    # Read the layout file and build the sheet.
    self.readSheet(self.character.getLayoutFile())

  def readSheet(self, layoutFile):
    ''' Read the XML file specified by layoutFile into a layout tree.
        '''
    self.dom = xml.dom.minidom.parse(layoutFile)

    # All character sheets must have atleast one <character_sheet> tag.
    if len(self.dom.getElementsByTagName('character_sheet')) < 1:
      print 'Tag Error: no <character_sheet>'
    else:
      node = self.dom.getElementsByTagName('character_sheet')[0]
      self.root = self.elements.character_sheet(node, self.character)
      # This list keeps a reference to all the objects in the sheet that can be edited.
      self.root.editables = []

      for child in node.childNodes:
	if child.nodeType is xml.dom.Node.ELEMENT_NODE:
	  self.root.packChild(self.makeObjects(child, self.root))

  def makeObjects(self, newNode, parent):
    ''' Make objects out of the node passed. '''
    # Tag represents a layout object.
    #try:
    newObject = self.elements.__dict__[newNode.tagName](newNode, self.character)

    # Should do better error handling
    #except KeyError:
      #print "Unknown tag:", newNode.tagName

    #else:
      # Every sheet element has an addEditable method, but if the object contains nothing
      # that can be edited the method does nothing.
    newObject.addEditable(self.root.editables)

      # Add all the children of the current node
    for node in newNode.childNodes:
      if node.nodeType is xml.dom.Node.ELEMENT_NODE:
        newObject.packChild(self.makeObjects(node, newObject))

      # If the new object needs to know about the action object then we'll add the action
      # object to the new object.
    if hasattr(newObject, 'addActionObject'):
      newObject.addActionObject(self.actionObject)

    return newObject
