""" LibCIA.Web.RegexTransform

Applies formatting to text in Nouvelle trees using regular expressions.
Each formatting class includes a dictionary of regular expressions mapped
to handler functions. When a regular expression is encountered, the handler
is called with the match object and the returned Nouvelle tree is inserted
in the original text's place.

This system can be used to auto-hyperlink URLs, format source code, and
probably much more.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2006 Micah Dowty <micah@navi.cx>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import re, types, Nouvelle
import Template


class RegexTransformBase(object):
    """Abstract base class for regex transformation engines. Subclasses
       must provide their own 'regexes' dictionary, mapping regex
       strings or compiled regexes to member functions. The constructor
       for this class walks through the class' ancestors and compiles
       regular expressions as necessary.
       """
    regexes = {}

    def __init__(self):
        # If this class doesn't have a compiled regex map, generate it
        if not hasattr(self.__class__, '_compiled'):
            self.__class__._compiled = self.compile()

    def compile(self):
        """Create a map of compiled regexes to handlers
           for this class and all its ancestors.
           """
        c = {}
        for cls in self.__class__.__mro__:
            # Stop when we get to this base class
            if cls is RegexTransformBase:
                break

            for uncompiled, handler in cls.regexes.iteritems():
                if type(uncompiled) in types.StringTypes:
                    compiled = re.compile(uncompiled)
                else:
                    compiled = uncompiled
                c[compiled] = handler
        return c

    def apply(self, tree, memo={}):
        """Recursively apply our compiled regexes to a Nouvelle tree,
           returning the transformed Nouvelle tree.
           """
        if type(tree) is tuple or type(tree) is list:
            return [self.apply(item, memo) for item in tree]

        elif type(tree) in types.StringTypes:
            # Yay, we found a string. Can we match any regexes?

            # Remember in the memo each regex that we process
            # so that we can avoid processing the same tree fragment
            # with this regex again later.
            memo = dict(memo)

            for regex, handler in self.__class__._compiled.iteritems():
                if regex in memo:
                    continue
                memo[regex] = 1

                results = []
                allStrings = True

                # For each regex, scan our string for matches.
                # Non-matched sections of the string and transformed
                # matches are accumulated into 'results'. If all
                # results are strings, the results are joined and
                # we can move on to the next regex immediately.
                # If not, we have to give this up now and recursively
                # transform each result.
                lastMatch = None
                for match in regex.finditer(tree):
                    # Store the fragment between the last match (if there was one)
                    # and the beginning of this match.
                    if lastMatch:
                        start = lastMatch.end()
                    else:
                        start = 0
                    results.append(tree[start:match.start()])

                    # Store the transformed result of this match.
                    # Note that we're passing self to handler!
                    # This is a bit of a hack since the handlers in
                    # self.regexes won't be bound methods normally.
                    transformed = handler(self, match)
                    results.append(transformed)
                    if type(transformed) not in types.StringTypes:
                        allStrings = False
                    lastMatch = match

                # Store the fragment between the last match and the end
                if lastMatch:
                    results.append(tree[lastMatch.end():])

                    # If we can join the results back into a string, do so
                    # and go ahead on to the next regex. If not, abort this
                    # and recursively process the results.
                    if allStrings:
                        tree = "".join(results)
                    else:
                        return [self.apply(item, memo) for item in results]
            return tree

        elif isinstance(tree, Nouvelle.tag):
            # Recursively process the tag's content
            return tree[self.apply(tree.content, memo)]

        # Anything else (hopefully a Nouvelle-serializable class) passes through unmodified
        return tree


class AutoHyperlink(RegexTransformBase):
    """A transform class that automatically hyperlinks all URLs.
       For Example:

       >>> linker = AutoHyperlink()
       >>> serial = Nouvelle.Serializer()

       >>> tree = linker.apply('Visit http://foo.bar today for a free toothbrush')
       >>> serial.render(tree)
       'Visit <a href="http://foo.bar">http://foo.bar</a> today for a free toothbrush'

       (This works for email addresses as well, but they aren't demonstrated here because
       the automatic email address obfuscation is not deterministic)
       """
    def link_url(self, match):
        url = match.group()
        return Nouvelle.tag('a', href=url)[ url ]

    def link_email(self, match):
        address = match.group()
        return Template.EmailLink("mailto:"+address)[ address ]

    regexes = {
        '(ht|f)tps?://[^\s\>\]\)]+':                    link_url,
        '[\w\.\-\+]+@([0-9a-zA-Z\-]+\.)+[a-zA-Z]+':     link_email,
        }

### The End ###
