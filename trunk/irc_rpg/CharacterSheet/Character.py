""" Character.py

This module loads the character data from the XML file.  It provides access
to all of the information in the file via XPaths.

Copyright (C) 2003 W. Evan Sheehan <evan@navi.picogui.org>
"""

import xml.dom.minidom, xml.xpath, string


class Character:
  """ Holds a dom of the character data. """
  def readCharacter(self, filename):
    """ Create a dom from an XML file. """
    self.filename = filename
    self.dom = xml.dom.minidom.parse(filename)

  def getData(self, path):
    """ Return the data from path. """
    nodes = xml.xpath.Evaluate(path, self.dom)
    if len(nodes) > 0:
      return nodes[0].childNodes[0].data.strip()
    return ""

  def getNode(self, path):
    """ Return the node at the end of the path. """
    nodes = xml.xpath.Evaluate(path, self.dom)
    if len(nodes) > 0:
      return nodes[0]

    return ""

  def setData(self, path, data):
    xml.xpath.Evaluate(path, self.dom)[0].childNodes[0].data = data

  def writeOut(self):
    """ Write the dom back out to the file. """
    file = open(self.filename, 'w+')
    self.dom.writexml(file)
