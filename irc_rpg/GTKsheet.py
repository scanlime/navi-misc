""" GTKsheet.py

Translate the layout objects in sheetLayout.py into GTK widgets.

Copyright (C) 2003 W. Evan Sheehan <evan@navi.picogui.org>
"""

import pygtk
from sheetLayout import sheetLayout

class GTKsheet:
  def readSheet(self, layoutFile):
    """ Read the XML file specified by layoutFile into a layout tree.
        """
    self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
    self.window.set_title('Character Sheet')

    self.box = gtk.HBox()

    self.dom = xml.dom.minidom.parse(layoutFile)

    # All character sheets must start with a <character_sheet> tag.
    if len(self.dom.getElementsByTagName('character_sheet')) != 1:
      print 'Tag Error'
    else:
      self.root =\
	  self.makeObjects(self.dom.getElementsByTagName('character_sheet')[0], None)
    self.box.show()
    self.window.show()

  def makeObjects(self, newNode, parent):
    """ Make objects out of the node passed. """
    # Tag represents a layout object.
    if hasattr(self, 'element_' + newNode.tagName):
      newObject = getattr(self, 'element_' + newNode.tagName)()
      #newObject.setAttributes(newNode)

      for node in newNode.childNodes:
	if node.nodeType is xml.dom.Node.ELEMENT_NODE:
	  newObject.addChild(self.makeObjects(node, newObject))
      return newObject

    # Tag is data for it's parent object.
    elif hasattr(parent, newNode.tagName):
      print newNode.childNodes
      for child in newNode.childNodes:
	parent.addData(child.data)

    # Should do better error handling
    else:
      print "Tag error:", newNode.tagName

  def element_tab_view(self):
    return gtk.NoteBook()

  def element_tab
class dice(gtk.Button):
