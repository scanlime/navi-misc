''' GTKsheetElements.py

Contains classes for making XML tags in the character sheets into gtk
widgets.

Copyright (C) 2003 W. Evan Sheehan <evan@navi.picogui.org>
'''

import gtk, re, traceback, string
from Character import Character
from random import randint

class sheetElement:
  ''' Superclass for all the tags. '''
  def __init__(self, node):
    self.attributes = self.getAttrs(node)

  def getAttrs(self, node):
    ''' Store all of the tags attributes in a dictionary with attribute names
        as the keys.
	'''
    return dict([(node.attributes.item(i).name,\
	node.getAttribute(node.attributes.item(i).name))\
	for i in range(node.attributes.length)])

  def addEditable(self, list):
    pass

class character_sheet(gtk.VBox, sheetElement):
  ''' Basic tag that's treated as a gtk.Window. '''
  def __init__(self, node, data):
    self.editables = []

    gtk.VBox.__init__(self, gtk.WINDOW_TOPLEVEL)
    sheetElement.__init__(self, node)

    # Toggle whether the fields are editable.
    self.edit = gtk.CheckButton('Editable')
    self.edit.connect('toggled', self.editable)

    # Write all fields out to the data file.
    self.apply = gtk.Button('Apply')
    self.apply.connect('clicked', self.applyChanges)

    box = gtk.HBox()
    box.pack_end(self.apply, gtk.FALSE, gtk.FALSE, 10)
    self.apply.show()
    box.pack_end(self.edit, gtk.FALSE, gtk.FALSE, 10)
    self.edit.show()

    self.pack_end(box, gtk.FALSE, gtk.FALSE, 10)
    box.show()

  def packChild(self, child):
    ''' Controls how to add children to the parent.  Children are added because
        character_sheet objects are treated like gtk.Window's.
	'''
    self.pack_start(child)
    child.show()

  def editable(self, widget=None, data=None):
    ''' Toggle the editable fields in the sheet. '''
    for field in self.editables:
      field.set_editable(self.edit.get_active())

  def applyChanges(self, widget=None, data=None):
    ''' Apply the changes in all of the fields to the data file. '''
    for field in self.editables:
      field.writeOut()

class tab_view(gtk.Notebook, sheetElement):
  ''' gtk.Notebook subclassed for tabbed elements in the character
      sheet.
      '''
  def __init__(self, node, data):
    sheetElement.__init__(self, node)
    gtk.Notebook.__init__(self)
    self.set_tab_pos(gtk.POS_TOP)

  def packChild(self, child):
    ''' Controls how to add children to the parent, children of the tab_view
        will be considered different pages for the gtk.Notebook and appended
	to it.
	'''
    gtk.Notebook.append_page(self, child, child.label)
    child.show()

class vbox(gtk.VBox, sheetElement):
  ''' Just a normal vertical box for packing. '''
  def __init__(self, node, data):
    sheetElement.__init__(self, node)
    gtk.VBox.__init__(self, homogeneous=gtk.TRUE)
    self.label = gtk.Label(self.attributes.get('label', ''))

  def packChild(self, child):
    ''' Just pack children into the vbox using pack_start because this is just
        a gtk.VBox.
	'''
    self.pack_start(child, expand=gtk.FALSE, padding=5)
    child.show()

class tab(vbox, sheetElement):
  ''' Treat tabs just like a vbox. '''
  def __init__(self, node, data):
    vbox.__init__(self, node, data)

class hbox(gtk.HBox, sheetElement):
  ''' Just a normal horizontal box for packing. '''
  def __init__(self, node, data):
    sheetElement.__init__(self, node)
    gtk.HBox.__init__(self)

  def packChild(self, child):
    ''' Pack children using pack_start, hbox is just a gtk.HBox. '''
    self.pack_start(child, padding=5)
    child.show()

class text_field(gtk.HBox, sheetElement):
  ''' The text_field widget is a combination of a gtk.Label and a
      gtk.Entry packed into a gtk.HBox.
      '''
  def __init__(self, node, characterData):
    sheetElement.__init__(self, node)
    gtk.HBox.__init__(self, spacing=5)

    self.label = gtk.Label(self.attributes.get('label', ''))
    self.text = gtk.Entry()
    self.text.set_text(characterData.getData(self.attributes.get('path', '')))
    self.text.set_editable(gtk.FALSE)
    self.pack_start(self.label)
    self.pack_start(self.text)

    self.characterData = characterData
    self.text.connect("activate", self.writeOut)

  def writeOut(self, widget=None, data=None):
    ''' Save the data in text field to the XML data file. '''
    self.characterData.setData(self.attributes['path'], self.text.get_text())
    self.characterData.writeOut()

  def show(self):
    ''' Provide a way to show this object identical to showing normal
        gtk widgets.
	'''
    self.text.show()
    self.label.show()
    gtk.HBox.show(self)

  def addEditable(self, list):
    ''' Add any editable fields to the list. '''
    list.append(self)

  def set_editable(self, is_editable):
    self.text.set_editable(is_editable)

class dice(gtk.Button, sheetElement):
  ''' gtk.Button subclass to add some additional information about
      the widget, such as a reference to the dom node that created
      it.
      '''
  def __init__(self, node, data, callback=lambda self: dice.roll(self)):
    sheetElement.__init__(self, node)
    gtk.Button.__init__(self, self.attributes.get('label', ''))
    self.node = node
    self.connect("clicked", callback)
    self.data = {'times':[], 'mods':[]}

  def packChild(self, child):
    ''' Children of a dice node will always be data for that node. '''
    self.data[child.name].append(child.data)

  def copyWithCallback(self, callback):
    ''' Return a copy of self but connected to a different callback method. '''
    newDie = dice(self.node, None, callback)
    newDie.node = self.node
    newDie.connect('activate', callback)
    newDie.data = self.data
    return newDie

  def roll(self, widget=None, data=None):
    ''' Roll the dice. '''
    rolls = []
    times = 0

    # Calculate the total number of times to roll the dice.
    for time in self.data['times']:
      times += time

    for i in range(times):
      roll = randint(1, int(self.attributes['sides']))
      # Add the modifiers to the roll.
      for mod in self.data['mods']:
	roll += mod
      rolls.append(roll)
    # For now, just print the results.
    print rolls


class times(sheetElement):
  ''' Number of times to roll the dice. '''
  def __init__(self, node, character):
    self.name = node.tagName
    if node.childNodes[0].data.count('/') > 0:
      self.data = int(character.getData(node.childNodes[0].data))
    else:
      self.data = int(node.childNodes[0].data)

class mods(times):
  ''' Modifiers to results, modifiers are exactly the same as times, just
      with a different name.
      '''
  def __init__(self, node, character):
    times.__init__(self, node, character)

class drop_down(hbox):
  ''' Drop down menu in the character sheet, can contain one dice button to roll dice determined
      by the menu.
      '''
  def __init__(self, node, character):
    hbox.__init__(self, node, character)
    #self.menu = []

    self.items = []
    self.characterData = character
    self.button = None

    # Drop down box.
    self.menu = gtk.Combo()
    self.menu.entry.set_editable(gtk.FALSE)
    self.pack_start(self.menu)

    # Text field for edit mode.
    self.editScroller = gtk.ScrolledWindow()
    self.editBox = gtk.TextView()
    self.buffer = self.editBox.get_buffer()
    frame = gtk.Frame()
    frame.add(self.editBox)
    frame.show()
    self.editScroller.add_with_viewport(frame)
    self.editBox.show()
    self.pack_start(self.editScroller)

    #for i in range(int(self.attributes.get('quantity', "1"))):
      #self.menu.append(gtk.Combo())
      #self.menu[i].connect("activate", self.setButton)
      #self.items.append({})

  def packChild(self, child):
    ''' If the child is a dice object copy it with a new callback and save
        a reference here.  Otherwise it's a menu item, so add it to the list
	of menu items. '''
    if isinstance(child, dice) and self.button is None:
      self.button = child.copyWithCallback(self.roll)
      self.pack_end(self.button, padding=5)
    elif isinstance(child, drop_down_item):
      for item in child.data:
        self.items.append(item)
      self.path = child.path
      self.menu.set_popdown_strings(self.items)

  def show(self):
    self.menu.show()
    # If there is a button, show it.
    if self.button is not None:
      self.button.show()
    hbox.show(self)

  def roll(self, widget=None, data=None):
    ''' Roll the dice object. '''
    # Get the data from the menu: number of times, number of sides and modifiers.
    values = re.search('\[([1-9]+)d([1-9]+)\+?([1-9]*)\]', self.menu.entry.get_text())
    if values:
      self.button.attributes['sides'] = values.group(2)
      self.button.data['times'] = [int(values.group(1))]
      if values.group(3):
	self.button.data['mods'] = [int(values.group(3))]
      self.button.roll()

  def writeOut(self):
    ''' Write out the data in the menu. '''
    self.characterData.setData(self.path, self.buffer.get_text(self.buffer.get_start_iter(), self.buffer.get_end_iter()))
    self.characterData.writeOut()

  def addEditable(self, list):
    ''' Add the entry field to the list of editable things. '''
    list.append(self)

  def set_editable(self, is_editable):
    ''' Make the drop down menu editable. '''
    if is_editable:
      self.buffer.set_text(string.join(self.items, '\n'))
      self.menu.hide()
      self.editScroller.show()
      if self.button is not None:
	self.button.hide()
    else:
      self.editScroller.hide()
      self.menu.show()
      self.items = self.buffer.get_text(self.buffer.get_start_iter(), self.buffer.get_end_iter()).split('\n')
      self.menu.set_popdown_strings(self.items)
      if self.button is not None:
	self.button.show()

class drop_down_item(sheetElement):
  ''' Items in a drop down menu. '''
  def __init__(self, node, character):
    sheetElement.__init__(self, node)

    # Get the data from the data sheet, split it on new lines and strip whitespace.
    if node.childNodes[0].data.count('/') > 0:
      self.data = character.getData(node.childNodes[0].data).split('\n')
      self.data = [item.strip() for item in self.data]
      self.path = node.childNodes[0].data.strip()
    # The data is constant in the layout sheet, so just save it.
    else:
      self.data = [node.childNodes[0].data]
      self.path = None

class text_box(gtk.Frame, sheetElement):
  ''' Text box for holding long strings of data. '''
  def __init__(self, node, character):
    sheetElement.__init__(self, node)
    gtk.Frame.__init__(self, self.attributes.get('label', ''))
    self.characterData = character
    # Text view.
    self.text = gtk.TextView()
    self.text.set_editable(gtk.FALSE)
    # Get the buffer.
    self.buffer = self.text.get_buffer()
    self.buffer.set_text(self.characterData.getData(self.attributes['path']))
    self.text.show()

    # Give the text view a scroller.
    scroller = gtk.ScrolledWindow()
    scroller.add_with_viewport(self.text)
    scroller.show()

    # The outer frame should be invisible except for the label.
    self.set_shadow_type(gtk.SHADOW_NONE)
    self.add(scroller)

  def addEditable(self, list):
    ''' Add the text box to the list of editable fields in the sheet. '''
    list.append(self)

  def set_editable(self, is_editable):
    self.text.set_editable(is_editable)

  def writeOut(self):
    ''' Write the data in the text box to the data file. '''
    self.characterData.setData(self.attributes['path'],\
	self.buffer.get_text(self.buffer.get_start_iter(), self.buffer.get_end_iter()))
    self.characterData.writeOut()
