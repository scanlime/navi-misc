#!/usr/bin/env python
'''
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

class Parser(HTMLParser):
  attrMap = ['name','price','power','muscle_req','description','unk']

  def __init__(self, html):
    HTMLParser.__init__(self)
    self.html = html
    self.items = []
    self.tdCount = 0
    self.currentItem = None

  def handle_starttag(self, tag, attrs):
    if tag == 'tr':
      self.currentItem = KoLItem()
      self.tdCount = 0

  def handle_endtag(self, tag):
    if tag == 'tr' and self.currentItem:
      self.items.append(self.currentItem)

    elif tag == 'td':
      self.tdCount += 1

  def handle_data(self, data):
    if self.currentItem and self.tdCount < 6:
      attr = getattr(self.currentItem, Parser.attrMap[self.tdCount])
      setattr(self.currentItem, Parser.attrMap[self.tdCount], string.join([attr, data.strip()]))

class KoLItem:
  def __init__(self):
    self.name = ""
    self.description = ""
    self.price = ""
    self.power = ""
    self.muscle_req = ""
    self.unk = ""

  def Print(self):
    print 'Item:\t\t%s\nDescription:\t\t%s\nPrice:\t\t%s\nPower:\t\t%s\nMuscle Req.:\t\t%s\nUnk:\t\t%s\n' % (self.name, self.description, self.price, self.power, self.muscle_req, self.unk)

if __name__ == '__main__':
  if len(sys.argv) != 2:
    print 'Usage: kol_parser.py <html file>'
    sys.exit()

  file = open(sys.argv[1], 'r')
  html = file.read()
  parser = Parser(file.readlines())
  parser.feed(html)
  for item in parser.items:
    item.Print()
