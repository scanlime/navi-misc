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
		""" Overridden from HTMLParser, called when a start tag is encountered.
				"""
		if self.get_starttag_text() == '<a href="/wordoftheday/">':
			self.word = data
			print self.word
