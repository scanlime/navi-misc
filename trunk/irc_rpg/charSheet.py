""" charSheet.py

Provides access to character sheets stored as XML.

Copyright (C) 2003 W. Evan Sheehan <evan@navi.picogui.org>
"""

import xml.dom.minidom
import xml.dom
import sheetElements


class sheetLayout:
  """ Python objects made from the XML file containing the layout
      information.
      """
  def readSheet(self, layoutFile):
    """ Read the XML file specified by layoutFile into a layout tree.
        """
    dom = xml.dom.minidom.parse(layoutFile)

    if len(dom.getElementsByTagName('character_sheet')) != 1:
      print 'Tag Error'
    else:
      self.root = dom.getElementsByTagName('character_sheet')[0]
      self.children = [self.makeObjects(node) for node in self.root.childNodes]

  def makeObjects(self, newNode):
    """ Make objects out of the node passed. """
    if newNode.nodeType is not xml.dom.Node.ELEMENT_NODE:
      return
    elif hasattr(sheetElements, newNode.tagName) and callable(getattr(sheetElements, newNode.tagName)):
      element = getattr(sheetElements, newNode.tagName)
      newObject = element()
      newObject.setAttributes(newNode)
      newObject.children = [self.makeObjects(node) for node in newNode.childNodes]
      return newObject
