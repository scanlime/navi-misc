""" webpageParser

Subclassed HTMLParser for parsing a web page by tag.
"""

#
# W. Evan Sheehan <evan@navi.picogui.org>
#

from HTMLParser import HTMLParser
import string

class wordoftheday(HTMLParser):
  """ Small subclass of HTMLParser that can search an html document
      for specific tags, and retrieve the data contained by the tags
      or that is surrounded by the tags.
      """
  def __init__(self):
    HTMLParser.__init__(self)
    self.word = ""
    self.isword = False

  def handle_data(self, data):
    """ Overridden from HTMLParser.  If the last tag grabbed is the tag
        for dictionary.com's word of the day.  Store it in self.word if
        the tags match and the data is longer than 1 character.
        """
    # Test the last tag touched and make sure the data is longer than a character
    # to insure it isn't garbage data.
    if self.get_starttag_text() == '<a href="/wordoftheday/">' and len(data) > 1:
      self.word = data
