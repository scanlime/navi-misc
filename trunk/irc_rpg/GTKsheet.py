""" GTKsheet.py

This module provides classes for turning an XML character sheet into gtk
widgets for a usable interface.

Copyright (C) 2003 W. Evan Sheehan <evan@navi.picogui.org>
"""

import xml.dom, xml.dom.minidom, gtk

class GTKsheet:
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
      newObject = globals()[newNode.tagName](newNode)
      #newObject.setAttributes(newNode)

      for node in newNode.childNodes:
	if node.nodeType is xml.dom.Node.ELEMENT_NODE:
	  newObject.packChild(self.makeObjects(node, newObject))
      return newObject

    # Tag is data for it's parent object.
    elif hasattr(parent, newNode.tagName):
      print "data handling not implemented"

    # Should do better error handling
    else:
      print "Tag error:", newNode.tagName

class character_sheet(gtk.Window):
  def __init__(self, node):
    gtk.Window.__init__(self)
    self.node = node

  def packChild(self, child):
    gtk.Window.add(self, child)
    child.show()

class tab_view(gtk.Notebook):
  """ gtk.Notebook subclassed for tabbed elements in the character
      sheet.
      """
  def __init__(self, node):
    gtk.Notebook.__init__(self)
    self.node = node
    self.set_tab_pos(gtk.POS_TOP)

  def packChild(self, child):
    gtk.Notebook.append_page(self, child, gtk.Label("bob"))
    child.show()

class tab(gtk.HBox):
  def __init__(self, node):
    gtk.HBox.__init__(self)
    self.node = node

  def packChild(self, child):
    gtk.HBox.pack_start(self, child)
    child.show()

class text_field(gtk.HBox):
  """ The text_field widget is a combination of a gtk.Label and a
      gtk.Entry packed into a gtk.HBox.
      """
  def __init__(self, node):
    gtk.HBox.__init__(self)
    self.label = gtk.Label()
    self.text = gtk.Entry()
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

class dice(gtk.Button):
  """ gtk.Button subclass to add some additional information about
      the widget, such as a reference to the dom node that created
      it.
      """
  def __init__(self, node):
    gtk.Button.__init__(self)
    self.node = node
    # Will this work?
    self.connect("clicked", self.roll)
    self.times = 0

  def roll(self, widget):
    print "roll not implemented"
