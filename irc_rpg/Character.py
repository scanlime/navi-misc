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
    self.dom = xml.dom.minidom.parse(filename)

  def getData(self, path):
    """ Return the data from path. """
    node = xml.xpath.Evaluate(path, self.dom)[0]
    return node.childNodes[0].data.strip()
