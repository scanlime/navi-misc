""" GTKsheetElements.py

Contains classes for making XML tags in the character sheets into gtk
widgets.

Copyright (C) 2003 W. Evan Sheehan <evan@navi.picogui.org>
"""

import gtk
from Character import Character


class sheetElement:
  """ Superclass for all the tags. """
  def __init__(self, node):
    self.attributes = self.getAttrs(node)

  def getAttrs(self, node):
    return dict([(node.attributes.item(i).name,\
	node.getAttribute(node.attributes.item(i).name))\
	for i in range(node.attributes.length)])

class character_sheet(gtk.Window, sheetElement):
  """ Basic tag that's treated as a gtk.Window. """
  def __init__(self, node, data):
    gtk.Window.__init__(self, gtk.WINDOW_TOPLEVEL)
    sheetElement.__init__(self, node)
    self.connect("delete_event", lambda w,d: self.destroy())

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
  """ Tabs are just treated as vertical boxes. """
  def __init__(self, node, data):
    sheetElement.__init__(self, node)
    gtk.VBox.__init__(self, homogeneous=gtk.TRUE)
    self.label = gtk.Label(self.attributes.get('label', ''))

  def packChild(self, child):
    gtk.VBox.pack_start(self, child, padding=5)
    child.show()

class hbox(gtk.HBox, sheetElement):
  """ Just a normal horizontal box for packing. """
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
    self.times = []

  def packChild(self, child):
    self.times.append(child.node.childNodes[0].data.strip())

  def roll(self, widget):
    for time in self.times:
      print time

class times:
  """ Data tags for dice tags. """
  def __init__(self, node, data):
    self.node = node
