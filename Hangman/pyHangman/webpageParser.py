""" webpageParser

Subclassed HTMLParser for parsing a web page by tag.
"""

#
# W. Evan Sheehan <evan@navi.picogui.org>
#

from HTMLParser import HTMLParser, HTMLParseError
import string

class webpageParser(HTMLParser):
	""" Small subclass of HTMLParser that can search an html document
			for specific tags, and retrieve the data contained by the tags
			or that is surrounded by the tags.
			"""
	def handle_starttag(self, tag, attrs):
		""" Overridden from HTMLParser, called when a start tag is encountered.
				"""


# It is highly likely none of the following will be kept.
	def findTags(self, tag, numTags=0):
		""" Return a list of all the positions at which 'tag' occurs.  Number of
				items returned can be limited by numTags, if numTags is 0 return all
				occurences found.
				"""

	def getAttributes(self, tag):
		""" Return a list of all of the attributes in the string 'tag'.
				e.g. <a href="blah"> would be returned as ['a', ('href', 'blah')]
				This allows the result of this function to be compared with the argument
				to handle_starttag() to determine if the tags are identical.
				"""
		tag = tag.strip('<>')
		attrs = tag.split()
		attrs = [self.attrSplit(attribute, '=') for attribute in attrs]
		return attrs

	def attrSplit(self, attribute, delimiter=string.whitespace):
		""" Split an html tag attriubte on 'delimiter'.  If 'delimiter' isn't present
				in 'attribute' just return 'attribute', otherwise return a tuple of the
				split attributes.
				"""
		if attribute.count(delimiter) == 0:
			return attribute

		attr1, attr2 = attribute.split(delimiter)
		return (attr1.strip(' "\''), attr2.strip(' "\''))
