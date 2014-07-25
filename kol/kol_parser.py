#!/usr/bin/env python
''' kol_parser.py

Python script that extracts the data from the pages of kol.quantumtunnel.net
and prepares it for the database for my KoL helper site.
'''
# Copyright (C) 2004 W. Evan Sheehan <evan@navi.cx>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import sys, string
from HTMLParser import HTMLParser
from optparse import OptionParser

class BaseParser(HTMLParser):
  ''' Base HTML parser for extracting KoL data from kol.quantumtunnel.net '''
  def __init__(self):
    HTMLParser.__init__(self)
    self.items = []           # List of items created from the file
    self.tdCount = 0          # Count of table columns so we know where we are
    self.currentItem = None   # Item we're currently creating

  def handle_endtag(self, tag):
    if tag == 'tr' and self.currentItem:
      self.items.append(self.currentItem)

    elif tag == 'td':
      self.tdCount += 1

  def handle_data(self, data):
    if self.currentItem and self.tdCount < len(self.__class__.attrMap):
      attr = getattr(self.currentItem, self.__class__.attrMap[self.tdCount])
      setattr(self.currentItem, self.__class__.attrMap[self.tdCount], string.join([attr, data.strip()]))

class ItemParser(BaseParser):
  ''' Item pages parser. '''
  attrMap = ['name','price','power','muscle_req','description','unk']

  def __init__(self):
    BaseParser.__init__(self)

  def handle_starttag(self, tag, attrs):
    if tag == 'tr':
      self.currentItem = KoLItem()
      self.tdCount = 0

class ConcParser(BaseParser):
  ''' Concoction pages parser. '''
  attrMap = ['name', 'directions', 'options']
  def __init__(self):
    BaseParser.__init__(self)

  def handle_starttag(self, tag, attrs):
    if tag == 'tr':
      self.currentItem = KoLConcoction()
      self.tdCount = 0

class EfctParser(BaseParser):
  ''' Effects pages parser. '''
  attrMap = ['num', 'effect', 'description']

  def __init__(self):
    BaseParser.__init__(self)

  def handle_starttag(self, tag, attrs):
    if tag == 'tr':
      self.currentItem = KoLEffect()
      self.tdCount = 0

class KoLItem:
  def __init__(self):
    self.name = ""
    self.description = ""
    self.price = ""
    self.power = ""
    self.muscle_req = ""
    self.unk = ""

  def Print(self):
    print 'Item: %s\nDescription: %s\nPrice: %s\nPower: %s\nMuscle Req.: %s\nUnk: %s\n' % (self.name, self.description, self.price, self.power, self.muscle_req, self.unk)

class KoLConcoction:
  def __init__(self):
    self.name = ''
    self.directions = ''
    self.options = ''

  def Print(self):
    print 'Item: %s\nDirections: %s\n' % (self.name, self.directions)

class KoLEffect:
  def __init__(self):
    self.num = ''
    self.effect = ''
    self.description = ''

  def Print(self):
    print 'Effect: %s\nDescription: %s\n' % (self.effect, self.description)

if __name__ == '__main__':
  ### Set up command line option parser ###
  usage = '''kol_parser.py parser_type file
  Parser Types:
    --item         for KoL items pages
    --concoctions  for KoL concoctions pages
    --effects      for KoL effects pages
  '''
  options = OptionParser(usage)
  options.add_option('--item', action='store_const', const='ItemParser', dest='parserType')
  options.add_option('--concoctions', action='store_const', const='ConcParser', dest='parserType')
  options.add_option('--effects', action='store_const', const='EfctParser', dest='parserType')

  if len(sys.argv) < 3:
    options.error('Too few arguments')

  (opts, args) = options.parse_args()

  # Open the html file.
  file = open(args[0])
  # Parse
  parser = eval(opts.parserType)()
  parser.feed(file.read())

  # Print
  for item in parser.items:
    item.Print()
