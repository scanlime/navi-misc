""" orpg_xml.py

This is a module for parsing the XML documents used to store data for
OpenRPG.

  Copyright (C) 2003 W. Evan Sheehan <evan@navi.picogui.org>
"""

from xmllib import xmllib
from xml.dom import minidom
import charSheet


# For deleting...
class orpg_xml(xmllib):
  """ Subclass xmllib for reading the data files. """
  def handle_starttag(tag, method, attributes):
    """ Handle start tags, if the tag can be made into an object do so.
        Otherwise it represents the data for some object, so store the
	tag name to determine which piece of data when you get it.
	"""
    if hasattr(charSheet, tag) and callable(getattr(charSheet, tag)):
      for tag in self.openTags:
	tag.addChild(tag
      self.openTags.append(charSheet.tag(attributes))
    else:
      self.dataTag = tag

  def handle_endtag(tag, method):
    """ Handle end tags, just take the last start tag off of the
        open tags stack, because it is no longer open.
	"""
    self.openTags.pop()

  def handle_data(data):
    """ Store the data in the object created by the last tag. """
    self.openTags[len(self.openTags) - 1].setAttr(self.dataTag, data)
