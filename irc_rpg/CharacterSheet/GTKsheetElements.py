""" GTKsheetElements.py

Contains classes for making XML tags in the character sheets into gtk
widgets.

Copyright (C) 2003 W. Evan Sheehan <evan@navi.picogui.org>
"""

import gtk, re
from Character import Character
from random import randint

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

class vbox(gtk.VBox, sheetElement):
  """ Just a normal vertical box for packing. """
  def __init__(self, node, data):
    sheetElement.__init__(self, node)
    gtk.VBox.__init__(self, homogeneous=gtk.TRUE)
    self.label = gtk.Label(self.attributes.get('label', ''))

  def packChild(self, child):
    gtk.VBox.pack_start(self, child, padding=5)
    child.show()

class tab(vbox, sheetElement):
  """ Treat tabs just like a vbox. """
  def __init__(self, node, data):
    vbox.__init__(self, node, data)

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
  def __init__(self, node, characterData):
    sheetElement.__init__(self, node)
    gtk.HBox.__init__(self, spacing=5)

    self.label = gtk.Label(self.attributes.get('label', ''))
    self.text = gtk.Entry()
    self.text.set_text(characterData.getData(self.attributes.get('path', '')))
    self.pack_start(self.label)
    self.pack_start(self.text)

    self.characterData = characterData
    self.text.connect("activate", self.writeOut)

  def writeOut(self, widget):
    self.characterData.setData(self.attributes['path'], self.text.get_text())
    self.characterData.writeOut()

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
  def __init__(self, node, data, callback=lambda self: dice.roll(self)):
    sheetElement.__init__(self, node)
    gtk.Button.__init__(self, self.attributes.get('label', ''))
    self.node = node
    self.connect("clicked", callback)
    self.data = {'times':[], 'mods':[]}

  def packChild(self, child):
    """ Children of a dice node will always be data for that node.
        """
    self.data[child.name].append(child.data)

  def copyWithCallback(self, callback):
    ''' Return a copy of self but connected to a different callback method. '''
    newDie = dice(self.node, None, callback)
    newDie.node = self.node
    newDie.connect('activate', callback)
    newDie.data = self.data
    return newDie

  def roll(self, widget=None):
    """ Roll the dice. """
    rolls = []
    times = 0

    for time in self.data['times']:
      times += time
    
    for i in range(times):
      roll = randint(1, int(self.attributes['sides']))
      for mod in self.data['mods']:
	roll += mod
      rolls.append(roll)
    print rolls

class times:
  """ Number of times to roll the dice. """
  def __init__(self, node, character):
    self.name = node.tagName
    if node.childNodes[0].data.count('/') > 0:
      self.data = int(character.getData(node.childNodes[0].data))
    else:
      self.data = int(node.childNodes[0].data)

class mods:
  """ Modifiers to results. """
  def __init__(self, node, character):
    self.name = node.tagName
    if node.childNodes[0].data.count('/') > 0:
      self.data = int(character.getData(node.childNodes[0].data))
    else:
      self.data = int(node.childNodes[0].data)

class drop_down(hbox):
  ''' Drop down menu in the character sheet, can contain one dice button to roll dice determined
      by the menu.
      '''
  def __init__(self, node, character):
    hbox.__init__(self, node, character)
    #self.menu = []
    self.items = {}
    self.button = None
    self.menu = gtk.Combo()
    self.pack_start(self.menu)
    self.beenPressed = False
    #for i in range(int(self.attributes.get('quantity', "1"))):
      #self.menu.append(gtk.Combo())
      #self.menu[i].connect("activate", self.setButton)
      #self.items.append({})

  def packChild(self, child):
    if isinstance(child, dice) and self.button is None:
      self.button = child.copyWithCallback(self.roll)
      self.pack_end(self.button)
    else:
      self.items[child.name] = child.data
      self.menu.set_popdown_strings(self.items.keys())

  def show(self):
    self.menu.show()
    if self.button is not None:
      self.button.show()
    hbox.show(self)

  def roll(self, widget=None):
    ''' Roll the dice object. '''
    values = re.search('\[([1-9]*)d([1-9]*)\]', self.menu.entry.get_text())
    if values is not None:
      if not self.beenPressed:
	self.beenPressed = True
	self.button.data['times'].append(int(values.group(1)))
	#self.button.data['mods'].append(int(values.group(3).strip('+')))
      else:
	self.button.data['times'][len(self.button.data['times']) - 1] = int(values.group(1))
	#self.button.data['mods'][len(self.button.data['mods'])] = values.group(3).strip('+')
      self.button.attributes['sides'] = values.group(2)
      dice.roll(self.button)

class drop_down_item(sheetElement):
  def __init__(self, node, character):
    sheetElement.__init__(self, node)

    if node.childNodes[0].data.count('/') > 0:
      self.data = int(character.getData(node.childNodes[0].data))
    else:
      self.data = node.childNodes[0].data

    self.name = self.attributes.get('label', self.data)
