""" Character.py

This module loads the character data from the XML file.  It provides access
to all of the information in the file via XPaths.

Copyright (C) 2003 W. Evan Sheehan <evan@navi.picogui.org>
"""

import xml.dom.minidom, xml.xpath, string


class Character:
  """ Holds a dom of the character data. """
  def __init__(self, filename=None):
    if filename:
      self.readCharacter(filename)

  def readCharacter(self, filename):
    """ Create a dom from an XML file. """
    self.filename = filename
    self.dom = xml.dom.minidom.parse(filename)

  def getData(self, path):
    """ Return the data from path. """
    nodes = xml.xpath.Evaluate(path, self.dom)
    if len(nodes) > 0:
      if len(nodes[0].childNodes) > 0: return nodes[0].childNodes[0].data.strip()
    else:
      self.makeNode(path)

    return ""

  def getNode(self, path):
    """ Return the node at the end of the path. """
    nodes = xml.xpath.Evaluate(path, self.dom)
    if len(nodes) > 0:
      return nodes[0]
    else:
      self.makeNode(path)

    return ""

  def makeNewDoc(self):
    ''' Create a new DOM for character data. '''
    self.dom = xml.dom.minidom.getDOMImplementation().createDocument(None, 'character', None)

  def makeNode(self, path):
    ''' Create the node at path. '''
    # Path to parent.
    parentPath = path[:path.rfind('/')]
    # Create new node and append a blank text node to it.
    node = self.dom.createElement(path[path.rfind('/')+1:])
    node.appendChild(self.dom.createTextNode(' '))
    # Get the parent node and append the new child.
    parent = xml.xpath.Evaluate(parentPath, self.dom)[0]
    parent.appendChild(node)
    parent.appendChild(self.dom.createTextNode('\n'))

  def setNodeAttr(self, path, attr):
    ''' Set the attribute(s) of the node at 'path' according to the dictionary 'attr'. '''
    node = xml.xpath.Evaluate(path, self.dom)[0]
    for key in attr.iterkeys():
      node.setAttribute(key, attr[key])

  def setData(self, path, data):
    ''' Set the data at the node denoted by path. '''
    nodes = xml.xpath.Evaluate(path, self.dom)
    # If the node doesn't exist create it and re-evalute the path.
    if len(nodes) is 0:
      self.makeNode(path)
      nodes = xml.xpath.Evaluate(path, self.dom)
    # If the node has a child set the data.
    if nodes[0].childNodes:
      nodes[0].childNodes[0].data = data
    # Otherwise create the child using 'data'.
    else:
      nodes[0].appendChild(self.dom.createTextNode(data))

  def writeOut(self):
    """ Write the dom back out to the file. """
    file = open(self.filename, 'w+')
    self.dom.writexml(file)

  def getLayoutFile(self):
    ''' Return the name of the layout file specified in the character sheet. '''
    return self.dom.getElementsByTagName('character')[0].getAttribute('layout')
