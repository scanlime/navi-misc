""" GTKsheet.py

This module provides classes for turning an XML character sheet into gtk
widgets for a usable interface.

Copyright (C) 2003 W. Evan Sheehan <evan@navi.picogui.org>
"""

import xml.dom, xml.dom.minidom, gtk
from Character import Character
class GTKsheet:
  def __init__(self, dataFile):
    self.character = Character()
    self.character.readCharacter(dataFile)
    self.readSheet(self.character.getData('/character/layoutSheet'))

  def readSheet(self, layoutFile):
    """ Read the XML file specified by layoutFile into a layout tree.
        """
    self.dom = xml.dom.minidom.parse(layoutFile)

    # All character sheets must start with a <character_sheet> tag.
    if len(self.dom.getElementsByTagName('character_sheet')) != 1:
      print 'Tag Error: no <character_sheet>'
    else:
      self.makeObjects(self.dom.getElementsByTagName('character_sheet')[0], None).show()

  def makeObjects(self, newNode, parent):
    """ Make objects out of the node passed. """
    # Tag represents a layout object.
    if globals().has_key(newNode.tagName):
      newObject = globals()[newNode.tagName](newNode, self.character)
      #newObject.setAttributes(newNode)

      for node in newNode.childNodes:
	if node.nodeType is xml.dom.Node.ELEMENT_NODE: newObject.packChild(self.makeObjects(node, newObject))
      return newObject

    # Tag is data for it's parent object.
    elif hasattr(parent, newNode.tagName):
      print "data handling not implemented"

    # Should do better error handling
    else:
      print "Tag error:", newNode.tagName

class sheetElement:
  def __init__(self, node):
    self.node = node
    self.attributes = self.getAttrs()

  def getAttrs(self):
    return dict([(self.node.attributes.item(i).name,\
	self.node.getAttribute(self.node.attributes.item(i).name))\
	for i in range(self.node.attributes.length)])

class character_sheet(gtk.Window, sheetElement):
  def __init__(self, node, data):
    gtk.Window.__init__(self, gtk.WINDOW_TOPLEVEL)
    sheetElement.__init__(self, node)
    self.connect("delete_event", lambda w,d: gtk.main_quit())

  def packChild(self, child):
    gtk.Window.add(self, child)
    child.show()

class tab_view(gtk.Notebook, sheetElement):
  """ gtk.Notebook subclassed for tabbed elements in the character
      sheet.
      """
  def __init__(self, node, data):
    sheetElement.__init__(self, node)
    gtk.Notebook.__init__(self)
    self.set_tab_pos(gtk.POS_TOP)

  def packChild(self, child):
    gtk.Notebook.append_page(self, child, child.label)
    child.show()

class tab(gtk.VBox, sheetElement):
  def __init__(self, node, data):
    sheetElement.__init__(self, node)
    gtk.VBox.__init__(self, homogeneous=gtk.TRUE)
    self.label = gtk.Label(self.attributes.get('label', ''))

  def packChild(self, child):
    gtk.VBox.pack_start(self, child, padding=5)
    child.show()

class hbox(gtk.HBox, sheetElement):
  def __init__(self, node, data):
    sheetElement.__init__(self, node)
    gtk.HBox.__init__(self)

  def packChild(self, child):
    gtk.HBox.pack_start(self, child, padding=5)
    child.show()

class text_field(gtk.HBox, sheetElement):
  """ The text_field widget is a combination of a gtk.Label and a
      gtk.Entry packed into a gtk.HBox.
      """
  def __init__(self, node, data):
    sheetElement.__init__(self, node)
    gtk.HBox.__init__(self, spacing=5)
    self.label = gtk.Label(self.attributes.get('label', ''))
    self.text = gtk.Entry()
    self.text.set_text(data.getData(self.attributes.get('path', '')))
    self.pack_start(self.label)
    self.pack_start(self.text)
    self.node = node

  def show(self):
    """ Provide a way to show this object identical to showing normal
        gtk widgets.
	"""
    self.text.show()
    self.label.show()
    gtk.HBox.show(self)

class dice(gtk.Button, sheetElement):
  """ gtk.Button subclass to add some additional information about
      the widget, such as a reference to the dom node that created
      it.
      """
  def __init__(self, node, data):
    sheetElement.__init__(self, node)
    gtk.Button.__init__(self, self.attributes.get('label', ''))
    self.node = node
    # Will this work?
    self.connect("clicked", self.roll)
    self.times = 0

  def roll(self, widget):
    print "roll not implemented"
